/* Aleth: Ethereum C++ client, tools and libraries.
 * Copyright 2018 Aleth Autors.
 * Licensed under the GNU General Public License, Version 3. See the LICENSE file.
 */

/**
 * Recursive Linear-Prefix serialization / deserialization.
 * @file
 */

#pragma once

#include "fixedhash.hpp"
#include "vector_ref.h"
#include "exception.h"
#include "common.h"
#include <array>
#include <exception>
#include <iomanip>
#include <iosfwd>
#include <vector>
#include <set>
#include <unordered_set>

namespace dipc
{

    class RLP;

    template <class _T> struct intTraits { static const unsigned maxSize = sizeof(_T); };
//    template <> struct intTraits<u160> { static const unsigned maxSize = 20; };
//    template <> struct intTraits<u256> { static const unsigned maxSize = 32; };
    template <> struct intTraits<bigint> { static const unsigned maxSize = ~(unsigned)0; };

    static const byte c_rlpMaxLengthBytes = 8;
    static const byte c_rlpDataImmLenStart = 0x80;
    static const byte c_rlpListStart = 0xc0;

    static const byte c_rlpDataImmLenCount = c_rlpListStart - c_rlpDataImmLenStart - c_rlpMaxLengthBytes;
    static const byte c_rlpDataIndLenZero = c_rlpDataImmLenStart + c_rlpDataImmLenCount - 1;
    static const byte c_rlpListImmLenCount = 256 - c_rlpListStart - c_rlpMaxLengthBytes;
    static const byte c_rlpListIndLenZero = c_rlpListStart + c_rlpListImmLenCount - 1;

    template <class T> struct Converter { static T convert(RLP const&, int) { dipcThrow("bad cast"); } };

/**
 * Class for interpreting Recursive Linear-Prefix Data.
 */
    class RLP
    {
    public:
        /// Conversion flags
        enum
        {
            AllowNonCanon = 1,
            ThrowOnFail = 4,
            FailIfTooBig = 8,
            FailIfTooSmall = 16,
            Strict = ThrowOnFail | FailIfTooBig,
            VeryStrict = ThrowOnFail | FailIfTooBig | FailIfTooSmall,
            LaissezFaire = AllowNonCanon
        };

        using Strictness = int;

        /// Construct a null node.
        RLP() {}

        /// Construct a node of value given in the bytes.
        explicit RLP(bytesConstRef _d, Strictness _s = VeryStrict) : m_data(_d) {
            if ((_s & FailIfTooBig) && actualSize() < _d.size())
            {
                if (_s & ThrowOnFail)
                    dipcThrow("over size rlp");
                else
                    m_data.reset();
            }
            if ((_s & FailIfTooSmall) && actualSize() > _d.size())
            {
                if (_s & ThrowOnFail)
                    dipcThrow("under size rlp");
                else
                    m_data.reset();
            }
        }

        /// Construct a node of value given in the bytes.
        explicit RLP(bytes const& _d, Strictness _s = VeryStrict): RLP(&_d, _s) {}

        /// Construct a node to read RLP data in the bytes given.
        RLP(byte const* _b, unsigned _s, Strictness _st = VeryStrict): RLP(bytesConstRef(_b, _s), _st) {}

        /// Construct a node to read RLP data in the string.
        explicit RLP(std::string const& _s, Strictness _st = VeryStrict): RLP(bytesConstRef((byte const*)_s.data(), _s.size()), _st) {}

        /// The bare data of the RLP.
        bytesConstRef data() const { return m_data; }

        /// @returns true if the RLP is non-null.
        explicit operator bool() const { return !isNull(); }

        /// No value.
        bool isNull() const { return m_data.size() == 0; }

        /// Contains a zero-length string or zero-length list.
        bool isEmpty() const { return !isNull() && (m_data[0] == c_rlpDataImmLenStart || m_data[0] == c_rlpListStart); }

        /// String value.
        bool isData() const { return !isNull() && m_data[0] < c_rlpListStart; }

        /// List value.
        bool isList() const { return !isNull() && m_data[0] >= c_rlpListStart; }

        /// Integer value. Must not have a leading zero.
        bool isInt() const {
            if (isNull())
                return false;
            requireGood();
            byte n = m_data[0];
            if (n < c_rlpDataImmLenStart)
                return !!n;
            else if (n == c_rlpDataImmLenStart)
                return true;
            else if (n <= c_rlpDataIndLenZero)
            {
                if (m_data.size() <= 1)
                    dipcThrow("bad rlp");
                return m_data[1] != 0;
            }
            else if (n < c_rlpListStart)
            {
                if (m_data.size() <= size_t(1 + n - c_rlpDataIndLenZero))
                    dipcThrow("bad rlp");
                return m_data[1 + n - c_rlpDataIndLenZero] != 0;
            }
            else
                return false;
            return false;
        }

        /// @returns the number of items in the list, or zero if it isn't a list.
        size_t itemCount() const { return isList() ? items() : 0; }
        size_t itemCountStrict() const { if (!isList()) dipcThrow("bad cast"); return items(); }

        /// @returns the number of bytes in the data, or zero if it isn't data.
        size_t size() const { return isData() ? length() : 0; }
        size_t sizeStrict() const { if (!isData()) dipcThrow("bad cast"); return length(); }

        /// Equality operators; does best-effort conversion and checks for equality.
        bool operator==(char const* _s) const { return isData() && toString() == _s; }
        bool operator!=(char const* _s) const { return isData() && toString() != _s; }
        bool operator==(std::string const& _s) const { return isData() && toString() == _s; }
        bool operator!=(std::string const& _s) const { return isData() && toString() != _s; }
        template <unsigned _N> bool operator==(FixedHash<_N> const& _h) const { return isData() && toHash<_N>() == _h; }
        template <unsigned _N> bool operator!=(FixedHash<_N> const& _s) const { return isData() && toHash<_N>() != _s; }
        bool operator==(unsigned const& _i) const { return isInt() && toInt<unsigned>() == _i; }
        bool operator!=(unsigned const& _i) const { return isInt() && toInt<unsigned>() != _i; }
        bool operator==(u256 const& _i) const { return isInt() && toInt<u256>() == _i; }
        bool operator!=(u256 const& _i) const { return isInt() && toInt<u256>() != _i; }
        bool operator==(bigint const& _i) const { return isInt() && toInt<bigint>() == _i; }
        bool operator!=(bigint const& _i) const { return isInt() && toInt<bigint>() != _i; }

        /// Subscript operator.
        /// @returns the list item @a _i if isList() and @a _i < listItems(), or RLP() otherwise.
        /// @note if used to access items in ascending order, this is efficient.
        RLP operator[](size_t _i) const {
            if (_i < m_lastIndex)
            {
                m_lastEnd = sizeAsEncoded(payload());
                m_lastItem = payload().cropped(0, m_lastEnd);
                m_lastIndex = 0;
            }
            for (; m_lastIndex < _i && m_lastItem.size(); ++m_lastIndex)
            {
                m_lastItem = payload().cropped(m_lastEnd);
                m_lastItem = m_lastItem.cropped(0, sizeAsEncoded(m_lastItem));
                m_lastEnd += m_lastItem.size();
            }
            return RLP(m_lastItem, int(ThrowOnFail) | int(FailIfTooSmall));
        }

        using element_type = RLP;

        /// @brief Iterator class for iterating through items of RLP list.
        class iterator
        {
            friend class RLP;

        public:
            using value_type = RLP;
            using element_type = RLP;

            iterator& operator++() {
                if (m_remaining)
                {
                    m_currentItem.retarget(m_currentItem.next().data(), m_remaining);
                    m_currentItem = m_currentItem.cropped(0, sizeAsEncoded(m_currentItem));
                    m_remaining -= std::min<size_t>(m_remaining, m_currentItem.size());
                }
                else
                    m_currentItem.retarget(m_currentItem.next().data(), 0);
                return *this;
            }
            iterator operator++(int) { auto ret = *this; operator++(); return ret; }
            RLP operator*() const { return RLP(m_currentItem); }
            bool operator==(iterator const& _cmp) const { return m_currentItem == _cmp.m_currentItem; }
            bool operator!=(iterator const& _cmp) const { return !operator==(_cmp); }

        private:
            iterator() {}
            iterator(RLP const& _parent, bool _begin) {
                if (_begin && _parent.isList())
                {
                    auto pl = _parent.payload();
                    m_currentItem = pl.cropped(0, sizeAsEncoded(pl));
                    m_remaining = pl.size() - m_currentItem.size();
                }
                else
                {
                    m_currentItem = _parent.data().cropped(_parent.data().size());
                    m_remaining = 0;
                }
            }

            size_t m_remaining = 0;
            bytesConstRef m_currentItem;
        };

        /// @brief Iterator into beginning of sub-item list (valid only if we are a list).
        iterator begin() const { return iterator(*this, true); }

        /// @brief Iterator into end of sub-item list (valid only if we are a list).
        iterator end() const { return iterator(*this, false); }

        template <class T> inline T convert(int _flags) const;

        /// Best-effort conversion operators.
        explicit operator std::string() const { return toString(); }
        explicit operator bytes() const { return toBytes(); }
        explicit operator uint8_t() const { return toInt<uint8_t>(); }
        explicit operator uint16_t() const { return toInt<uint16_t>(); }
        explicit operator uint32_t() const { return toInt<uint32_t>(); }
        explicit operator uint64_t() const { return toInt<uint64_t>(); }
        explicit operator u160() const { return toInt<u160>(); }
        explicit operator u256() const { return toInt<u256>(); }
        explicit operator bigint() const { return toInt<bigint>(); }
        template <unsigned N> explicit operator FixedHash<N>() const { return toHash<FixedHash<N>>(); }
        template <class T, class U> explicit operator std::pair<T, U>() const { return toPair<T, U>(); }
        template <class T> explicit operator std::vector<T>() const { return toVector<T>(); }
        template <class T> explicit operator std::set<T>() const { return toSet<T>(); }
        template <class T, size_t N> explicit operator std::array<T, N>() const { return toArray<T, N>(); }

        /// Converts to bytearray. @returns the empty byte array if not a string.
        bytes toBytes(int _flags = LaissezFaire) const { if (!isData()) { if (_flags & ThrowOnFail) dipcThrow("bad cast"); else return bytes(); } return bytes(payload().data(), payload().data() + length()); }
        /// Converts to bytearray. @returns the empty byte array if not a string.
        bytesConstRef toBytesConstRef(int _flags = LaissezFaire) const { if (!isData()) { if (_flags & ThrowOnFail) dipcThrow("bad cast"); else return bytesConstRef(); } return payload().cropped(0, length()); }
        /// Converts to string. @returns the empty string if not a string.
        std::string toString(int _flags = LaissezFaire) const { if (!isData()) { if (_flags & ThrowOnFail) dipcThrow("bad cast"); else return std::string(); } return payload().cropped(0, length()).toString(); }
        /// Converts to string. @throws BadCast if not a string.
        std::string toStringStrict() const { return toString(Strict); }

        template <class T>
        std::vector<T> toVector(int _flags = LaissezFaire) const
        {
            std::vector<T> ret;
            if (isList())
            {
                ret.reserve(itemCount());
                for (auto const& i: *this)
                    ret.push_back(i.convert<T>(_flags));
            }
            else if (_flags & ThrowOnFail)
                dipcThrow("bad cast");
            return ret;
        }

        template <class T>
        std::set<T> toSet(int _flags = LaissezFaire) const
        {
            std::set<T> ret;
            if (isList())
                for (auto const& i: *this)
                    ret.insert(i.convert<T>(_flags));
            else if (_flags & ThrowOnFail)
                dipcThrow("bad cast");
            return ret;
        }

        template <class T>
        std::unordered_set<T> toUnorderedSet(int _flags = LaissezFaire) const
        {
            std::unordered_set<T> ret;
            if (isList())
                for (auto const& i: *this)
                    ret.insert(i.convert<T>(_flags));
            else if (_flags & ThrowOnFail)
                dipcThrow("bad cast");
            return ret;
        }

        template <class T, class U>
        std::pair<T, U> toPair(int _flags = Strict) const
        {
            std::pair<T, U> ret;
            if (itemCountStrict() != 2)
            {
                if (_flags & ThrowOnFail)
                    dipcThrow("bad cast");
                else
                    return ret;
            }
            ret.first = (*this)[0].convert<T>(_flags);
            ret.second = (*this)[1].convert<U>(_flags);
            return ret;
        }

        template <class T, size_t N>
        std::array<T, N> toArray(int _flags = LaissezFaire) const
        {
            if (itemCountStrict() != N)
            {
                if (_flags & ThrowOnFail)
                    dipcThrow("bad cast");
                else
                    return std::array<T, N>();
            }
            std::array<T, N> ret;
            for (size_t i = 0; i < N; ++i)
                ret[i] = operator[](i).convert<T>(_flags);
            return ret;
        }

        /// Converts to int of type given; if isData(), decodes as big-endian bytestream. @returns 0 if not an int or data.
        template <class _T = unsigned> _T toInt(int _flags = Strict) const
        {
            requireGood();
            if ((!isInt() && !(_flags & AllowNonCanon)) || isList() || isNull())
            {
                if (_flags & ThrowOnFail)
                    dipcThrow("bad cast");
                else
                    return 0;
            }

            auto p = payload();
            if (p.size() > intTraits<_T>::maxSize && (_flags & FailIfTooBig))
            {
                if (_flags & ThrowOnFail)
                    dipcThrow("bad cast");
                else
                    return 0;
            }

            return fromBigEndian<_T>(p);
        }

        int64_t toPositiveInt64(int _flags = Strict) const
        {
            int64_t i = toInt<int64_t>(_flags);
            if ((_flags & ThrowOnFail) && i < 0)
                dipcThrow("bad cast");
            return i;
        }

        template <class _N> _N toHash(int _flags = Strict) const
        {
            requireGood();
            auto p = payload();
            auto l = p.size();
            if (!isData() || (l > _N::size && (_flags & FailIfTooBig)) || (l < _N::size && (_flags & FailIfTooSmall)))
            {
                if (_flags & ThrowOnFail)
                    dipcThrow("bad cast");
                else
                    return _N();
            }

            _N ret;
            size_t s = std::min<size_t>(_N::size, l);
            memcpy(ret.data() + _N::size - s, p.data(), s);
            return ret;
        }

        /// @returns the data payload. Valid for all types.
        bytesConstRef payload() const { auto l = length(); if (l > m_data.size()) dipcThrow("bad cast"); return m_data.cropped(payloadOffset(), l); }

        /// @returns the theoretical size of this item as encoded in the data.
        /// @note Under normal circumstances, is equivalent to m_data.size() - use that unless you know it won't work.
        size_t actualSize() const {
            if (isNull())
                return 0;
            if (isSingleByte())
                return 1;
            if (isData() || isList())
                return payloadOffset() + length();
            return 0;
        }

    private:
        /// Disable construction from rvalue
        explicit RLP(bytes const&&) {}

        /// Throws if is non-canonical data (i.e. single byte done in two bytes that could be done in one).
        void requireGood() const {
            if (isNull())
                dipcThrow("bad rlp");
            byte n = m_data[0];
            if (n != c_rlpDataImmLenStart + 1)
                return;
            if (m_data.size() < 2)
                dipcThrow("bad rlp");
            if (m_data[1] < c_rlpDataImmLenStart)
                dipcThrow("bad rlp");
        }

        /// Single-byte data payload.
        bool isSingleByte() const { return !isNull() && m_data[0] < c_rlpDataImmLenStart; }

        /// @returns the amount of bytes used to encode the length of the data. Valid for all types.
        unsigned lengthSize() const { if (isData() && m_data[0] > c_rlpDataIndLenZero) return m_data[0] - c_rlpDataIndLenZero; if (isList() && m_data[0] > c_rlpListIndLenZero) return m_data[0] - c_rlpListIndLenZero; return 0; }

        /// @returns the size in bytes of the payload, as given by the RLP as opposed to as inferred from m_data.
        size_t length() const {
            if (isNull())
                return 0;
            requireGood();
            size_t ret = 0;
            byte const n = m_data[0];
            if (n < c_rlpDataImmLenStart)
                return 1;
            else if (n <= c_rlpDataIndLenZero)
                return n - c_rlpDataImmLenStart;
            else if (n < c_rlpListStart)
            {
                if (m_data.size() <= size_t(n - c_rlpDataIndLenZero))
                    dipcThrow("bad rlp");
                if (m_data.size() > 1)
                    if (m_data[1] == 0)
                        dipcThrow("bad rlp");
                unsigned lengthSize = n - c_rlpDataIndLenZero;
                if (lengthSize > sizeof(ret))
                    // We did not check, but would most probably not fit in our memory.
                    dipcThrow("under size rlp");
                // No leading zeroes.
                if (!m_data[1])
                    dipcThrow("bad rlp");
                for (unsigned i = 0; i < lengthSize; ++i)
                    ret = (ret << 8) | m_data[i + 1];
                // Must be greater than the limit.
                if (ret < c_rlpListStart - c_rlpDataImmLenStart - c_rlpMaxLengthBytes)
                    dipcThrow("bad rlp");
            }
            else if (n <= c_rlpListIndLenZero)
                return n - c_rlpListStart;
            else
            {
                unsigned lengthSize = n - c_rlpListIndLenZero;
                if (m_data.size() <= lengthSize)
                    dipcThrow("bad rlp");
                if (m_data.size() > 1)
                    if (m_data[1] == 0)
                        dipcThrow("bad rlp");
                if (lengthSize > sizeof(ret))
                    // We did not check, but would most probably not fit in our memory.
                    dipcThrow("under size rlp");
                if (!m_data[1])
                    dipcThrow("bad rlp");
                for (unsigned i = 0; i < lengthSize; ++i)
                    ret = (ret << 8) | m_data[i + 1];
                if (ret < 0x100 - c_rlpListStart - c_rlpMaxLengthBytes)
                    dipcThrow("bad rlp");
            }
            // We have to be able to add payloadOffset to length without overflow.
            // This rejects roughly 4GB-sized RLPs on some platforms.
            if (ret >= std::numeric_limits<size_t>::max() - 0x100)
                dipcThrow("under size rlp");
            return ret;
        }

        /// @returns the number of bytes into the data that the payload starts.
        size_t payloadOffset() const { return isSingleByte() ? 0 : (1 + lengthSize()); }

        /// @returns the number of data items.
        size_t items() const {
            if (isList())
            {
                bytesConstRef d = payload();
                size_t i = 0;
                for (; d.size(); ++i)
                    d = d.cropped(sizeAsEncoded(d));
                return i;
            }
            return 0;
        }

        /// @returns the size encoded into the RLP in @a _data and throws if _data is too short.
        static size_t sizeAsEncoded(bytesConstRef _data) { return RLP(_data, int(ThrowOnFail) | int(FailIfTooSmall)).actualSize(); }

        /// Our byte data.
        bytesConstRef m_data;

        /// The list-indexing cache.
        mutable size_t m_lastIndex = (size_t)-1;
        mutable size_t m_lastEnd = 0;
        mutable bytesConstRef m_lastItem;
    };

    template <> struct Converter<std::string> { static std::string convert(RLP const& _r, int _flags) { return _r.toString(_flags); } };
    template <> struct Converter<bytes> { static bytes convert(RLP const& _r, int _flags) { return _r.toBytes(_flags); } };
    template <> struct Converter<uint8_t> { static uint8_t convert(RLP const& _r, int _flags) { return _r.toInt<uint8_t>(_flags); } };
    template <> struct Converter<uint16_t> { static uint16_t convert(RLP const& _r, int _flags) { return _r.toInt<uint16_t>(_flags); } };
    template <> struct Converter<uint32_t> { static uint32_t convert(RLP const& _r, int _flags) { return _r.toInt<uint32_t>(_flags); } };
    template <> struct Converter<uint64_t> { static uint64_t convert(RLP const& _r, int _flags) { return _r.toInt<uint64_t>(_flags); } };
    template <> struct Converter<u160> { static u160 convert(RLP const& _r, int _flags) { return _r.toInt<u160>(_flags); } };
    template <> struct Converter<u256> { static u256 convert(RLP const& _r, int _flags) { return _r.toInt<u256>(_flags); } };
    template <> struct Converter<bigint> { static bigint convert(RLP const& _r, int _flags) { return _r.toInt<bigint>(_flags); } };
    template <unsigned N> struct Converter<FixedHash<N>> { static FixedHash<N> convert(RLP const& _r, int _flags) { return _r.toHash<FixedHash<N>>(_flags); } };
template <class T, class U> struct Converter<std::pair<T, U>> { static std::pair<T, U> convert(RLP const& _r, int _flags) { return _r.toPair<T, U>(_flags); } };
template <class T> struct Converter<std::vector<T>> { static std::vector<T> convert(RLP const& _r, int _flags) { return _r.toVector<T>(_flags); } };
template <class T> struct Converter<std::set<T>> { static std::set<T> convert(RLP const& _r, int _flags) { return _r.toSet<T>(_flags); } };
template <class T> struct Converter<std::unordered_set<T>> { static std::unordered_set<T> convert(RLP const& _r, int _flags) { return _r.toUnorderedSet<T>(_flags); } };
template <class T, size_t N> struct Converter<std::array<T, N>> { static std::array<T, N> convert(RLP const& _r, int _flags) { return _r.toArray<T, N>(_flags); } };

template <class T> inline T RLP::convert(int _flags) const { return Converter<T>::convert(*this, _flags); }

/**
 * @brief Class for writing to an RLP bytestream.
 */
class RLPStream
{
public:
    /// Initializes empty RLPStream.
    RLPStream() {}

    /// Initializes the RLPStream as a list of @a _listItems items.
    explicit RLPStream(size_t _listItems) { appendList(_listItems); }

    ~RLPStream() {}

    /// Append given datum to the byte stream.
    RLPStream& append(unsigned _s) { return append(bigint(_s)); }
    RLPStream& append(u160 _s) { return append(bigint(_s)); }
    RLPStream& append(u256 _s) { return append(bigint(_s)); }
    RLPStream& append(bigint _i) {
        if (!_i)
            m_out.push_back(c_rlpDataImmLenStart);
        else if (_i < c_rlpDataImmLenStart)
            m_out.push_back((byte)_i);
        else
        {
            unsigned br = bytesRequired(_i);
            if (br < c_rlpDataImmLenCount)
                m_out.push_back((byte)(br + c_rlpDataImmLenStart));
            else
            {
                auto brbr = bytesRequired(br);
                if (c_rlpDataIndLenZero + brbr > 0xff)
                    dipcThrow("Number too large for RLP");
                m_out.push_back((byte)(c_rlpDataIndLenZero + brbr));
                pushInt(br, brbr);
            }
            pushInt(_i, br);
        }
        noteAppended();
        return *this;
    }
    RLPStream& append(bytesConstRef _s, bool _compact = false) {
        size_t s = _s.size();
        byte const* d = _s.data();
        if (_compact)
            for (size_t i = 0; i < _s.size() && !*d; ++i, --s, ++d) {}

        if (s == 1 && *d < c_rlpDataImmLenStart)
            m_out.push_back(*d);
        else
        {
            if (s < c_rlpDataImmLenCount)
                m_out.push_back((byte)(s + c_rlpDataImmLenStart));
            else
                pushCount(s, c_rlpDataIndLenZero);
            appendRaw(bytesConstRef(d, s), 0);
        }
        noteAppended();
        return *this;
    }
    RLPStream& append(bytes const& _s) { return append(bytesConstRef(&_s)); }
    RLPStream& append(std::string const& _s) { return append(bytesConstRef(_s)); }
    RLPStream& append(char const* _s) { return append(std::string(_s)); }
    template <unsigned N> RLPStream& append(FixedHash<N> _s, bool _compact = false, bool _allOrNothing = false) { return _allOrNothing && !_s ? append(bytesConstRef()) : append(_s.ref(), _compact); }

    /// Appends an arbitrary RLP fragment - this *must* be a single item unless @a _itemCount is given.
    RLPStream& append(RLP const& _rlp, size_t _itemCount = 1) { return appendRaw(_rlp.data(), _itemCount); }

    /// Appends a sequence of data to the stream as a list.
    template <class _T> RLPStream& append(std::vector<_T> const& _s) { return appendVector(_s); }
    template <class _T> RLPStream& appendVector(std::vector<_T> const& _s) { appendList(_s.size()); for (auto const& i: _s) append(i); return *this; }
    template <class _T, size_t S> RLPStream& append(std::array<_T, S> const& _s) { appendList(_s.size()); for (auto const& i: _s) append(i); return *this; }
    template <class _T> RLPStream& append(std::set<_T> const& _s) { appendList(_s.size()); for (auto const& i: _s) append(i); return *this; }
    template <class _T> RLPStream& append(std::unordered_set<_T> const& _s) { appendList(_s.size()); for (auto const& i: _s) append(i); return *this; }
    template <class T, class U> RLPStream& append(std::pair<T, U> const& _s) { appendList(2); append(_s.first); append(_s.second); return *this; }

    /// Appends a list.
    RLPStream& appendList(size_t _items) {
        //	cdebug << "appendList(" << _items << ")";
        if (_items)
            m_listStack.push_back(std::make_pair(_items, m_out.size()));
        else
            appendList(bytes());
        return *this;
    }
    RLPStream& appendList(bytesConstRef _rlp) {
        if (_rlp.size() < c_rlpListImmLenCount)
            m_out.push_back((byte)(_rlp.size() + c_rlpListStart));
        else
            pushCount(_rlp.size(), c_rlpListIndLenZero);
        appendRaw(_rlp, 1);
        return *this;
    }
    RLPStream& appendList(bytes const& _rlp) { return appendList(&_rlp); }
    RLPStream& appendList(RLPStream const& _s) { return appendList(&_s.out()); }

    /// Appends raw (pre-serialised) RLP data. Use with caution.
    RLPStream& appendRaw(bytesConstRef _s, size_t _itemCount = 1) {
        m_out.insert(m_out.end(), _s.begin(), _s.end());
        noteAppended(_itemCount);
        return *this;
    }
    RLPStream& appendRaw(bytes const& _rlp, size_t _itemCount = 1) { return appendRaw(&_rlp, _itemCount); }

    /// Shift operators for appending data items.
    template <class T> RLPStream& operator<<(T _data) { return append(_data); }

    /// Clear the output stream so far.
    void clear() { m_out.clear(); m_listStack.clear(); }

    /// Read the byte stream.
    bytes const& out() const { if(!m_listStack.empty()) dipcThrow("listStack is not empty"); return m_out; }

    /// Invalidate the object and steal the output byte stream.
    bytes&& invalidate() { if(!m_listStack.empty()) dipcThrow("listStack is not empty"); return std::move(m_out); }

    /// Swap the contents of the output stream out for some other byte array.
    void swapOut(bytes& _dest) { if(!m_listStack.empty()) dipcThrow("listStack is not empty"); swap(m_out, _dest); }

private:
    void noteAppended(size_t _itemCount = 1) {
        if (!_itemCount)
            return;
    //	cdebug << "noteAppended(" << _itemCount << ")";
        while (m_listStack.size())
        {
            if (m_listStack.back().first < _itemCount)
                dipcThrow("itemCount too large");
            m_listStack.back().first -= _itemCount;
            if (m_listStack.back().first)
                break;
            else
            {
                auto p = m_listStack.back().second;
                m_listStack.pop_back();
                size_t s = m_out.size() - p;		// list size
                auto brs = bytesRequired(s);
                unsigned encodeSize = s < c_rlpListImmLenCount ? 1 : (1 + brs);
    //			cdebug << "s: " << s << ", p: " << p << ", m_out.size(): " << m_out.size() << ", encodeSize: " << encodeSize << " (br: " << brs << ")";
                auto os = m_out.size();
                m_out.resize(os + encodeSize);
                memmove(m_out.data() + p + encodeSize, m_out.data() + p, os - p);
                if (s < c_rlpListImmLenCount)
                    m_out[p] = (byte)(c_rlpListStart + s);
                else if (c_rlpListIndLenZero + brs <= 0xff)
                {
                    m_out[p] = (byte)(c_rlpListIndLenZero + brs);
                    byte* b = &(m_out[p + brs]);
                    for (; s; s >>= 8)
                        *(b--) = (byte)s;
                }
                else
                    dipcThrow("itemCount too large for RLP");
            }
            _itemCount = 1;	// for all following iterations, we've effectively appended a single item only since we completed a list.
        }
    }

    /// Push the node-type byte (using @a _base) along with the item count @a _count.
    /// @arg _count is number of characters for strings, data-bytes for ints, or items for lists.
    void pushCount(size_t _count, byte _base) {
        auto br = bytesRequired(_count);
        if (int(br) + _base > 0xff)
            dipcThrow("Count too large for RLP");
        m_out.push_back((byte)(br + _base));	// max 8 bytes.
        pushInt(_count, br);
    }

    /// Push an integer as a raw big-endian byte-stream.
    template <class _T> void pushInt(_T _i, size_t _br)
    {
        m_out.resize(m_out.size() + _br);
        byte* b = &m_out.back();
        for (; _i; _i >>= 8)
            *(b--) = (byte)_i;
    }

    /// Our output byte stream.
    bytes m_out;

    std::vector<std::pair<size_t, size_t>> m_listStack;
};

template <class _T> void rlpListAux(RLPStream& _out, _T _t) { _out << _t; }
template <class _T, class ... _Ts> void rlpListAux(RLPStream& _out, _T _t, _Ts ... _ts) { rlpListAux(_out << _t, _ts...); }

/// Export a single item in RLP format, returning a byte array.
template <class _T> bytes rlp(_T _t) { return (RLPStream() << _t).out(); }

/// Export a list of items in RLP format, returning a byte array.
inline bytes rlpList() { return RLPStream(0).out(); }
template <class ... _Ts> bytes rlpList(_Ts ... _ts)
{
    RLPStream out(sizeof ...(_Ts));
    rlpListAux(out, _ts...);
    return out.out();
}

/// The empty string in RLP format.
//extern bytes RLPNull;

/// The empty list in RLP format.
//extern bytes RLPEmptyList;

//bytes RLPNull = rlp("");
//bytes RLPEmptyList = rlpList();

/// Human readable version of RLP.
//std::ostream& operator<<(std::ostream& _out, dev::RLP const& _d);

}
