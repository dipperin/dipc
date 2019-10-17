#pragma once

#include <vector>
#include <string>
#include <array>
#include <initializer_list>
#include "common.h"
//#include "commonData.hpp"

namespace dipc {

    template <unsigned N>
    class FixedHash{
    public:
        /// Construct an empty hash.
        FixedHash() { m_data.fill(0); }

        template <unsigned M> explicit FixedHash(FixedHash<M> const& h){
            m_data.fill(0);
            unsigned c = std::min(M, N);
            for (unsigned i = 0; i < c; ++i){
                m_data[i] = h[i];
            }
        }
        template <typename T>
        FixedHash(const std::initializer_list<T> &l) {
            m_data.fill(0);
            unsigned i= 0;
            for (auto it = l.begin(); it != l.end() && i < N; ++it, ++i) {
                m_data[i] = *it;
            }
        }

        explicit FixedHash(unsigned _u) { toBigEndian(_u, m_data); }

        explicit FixedHash(const byte *h, size_t len){
            std::cout << "this function is called!" << std::endl;
            memcpy(m_data.data(), h, N);
        }
        explicit FixedHash(const bytes &&b) {
            std::cout << "this function parameter 'const bytes &&b' is called!" << std::endl;
            std::copy(b.begin(), b.end(), m_data.begin());
        }

        explicit FixedHash(const std::string &s, bool isHex = true): FixedHash(isHex ? fromHex(s) : asBytes(s)){}

        std::string toString() const {
            return toHex(m_data);
        }
        byte const* data() const { return m_data.data(); }
        byte* data() { return m_data.data(); }
        bytesRef ref() { return bytesRef(m_data.data(), N); }
        /// @returns true iff this is the empty hash.
        explicit operator bool() const { return std::any_of(m_data.begin(), m_data.end(), [](byte _b) { return _b != 0; }); }

        // The obvious comparison operators.
        bool operator==(FixedHash const& c) const { return m_data == c.m_data; }
        bool operator!=(FixedHash const& c) const { return m_data != c.m_data; }
        bool operator<(FixedHash const& c) const { for (unsigned i = 0; i < N; ++i) if (m_data[i] < c.m_data[i]) return true; else if (m_data[i] > c.m_data[i]) return false; return false; }
        bool operator>=(FixedHash const& c) const { return !operator<(c); }
        bool operator<=(FixedHash const& c) const { return operator==(c) || operator<(c); }
        bool operator>(FixedHash const& c) const { return !operator<=(c); }

        // The obvious binary operators.
        FixedHash& operator^=(FixedHash const& c) { for (unsigned i = 0; i < N; ++i) m_data[i] ^= c.m_data[i]; return *this; }
        FixedHash operator^(FixedHash const& c) const { return FixedHash(*this) ^= c; }
        FixedHash& operator|=(FixedHash const& c) { for (unsigned i = 0; i < N; ++i) m_data[i] |= c.m_data[i]; return *this; }
        FixedHash operator|(FixedHash const& c) const { return FixedHash(*this) |= c; }
        FixedHash& operator&=(FixedHash const& c) { for (unsigned i = 0; i < N; ++i) m_data[i] &= c.m_data[i]; return *this; }
        FixedHash operator&(FixedHash const& c) const { return FixedHash(*this) &= c; }
        FixedHash operator~() const { FixedHash ret; for (unsigned i = 0; i < N; ++i) ret[i] = ~m_data[i]; return ret; }

        /// @returns a particular byte from the hash.
        byte& operator[](unsigned _i) { return m_data[_i]; }
        /// @returns a particular byte from the hash.
        byte operator[](unsigned _i) const { return m_data[_i]; }

        // Big-endian increment.
        FixedHash& operator++() { for (unsigned i = size(); i > 0 && !++m_data[--i]; ) {} return *this; }

        /// @returns true if all one-bits in @a _c are set in this object.
        bool contains(FixedHash const& _c) const { return (*this & _c) == _c; }

        const size_t size() const { return N; }
    private:
        std::array<byte, N> m_data;
    };

    using h256 = FixedHash<32>;
    using h160 = FixedHash<20>;
    using h128 = FixedHash<16>;
    using h64 = FixedHash<8>;
    //using Address = FixedHash<20>;
    using Address = FixedHash<22>;


    inline std::string CharToAddress2Str(const char* own){
        bytes result = fromHex(own);
        Address owner = Address(&result[0], 22);
        return owner.toString();
    }

    inline std::string StrToAddress2Str(std::string own){
        bytes result = fromHex(own);
        Address owner = Address(&result[0], 22);
        return owner.toString();
    }



    //  copy  from eth
//     template <unsigned T>
// class SecureFixedHash: private FixedHash<T>
// {
// public:
// 	using ConstructFromHashType = typename FixedHash<T>::ConstructFromHashType;
// 	using ConstructFromStringType = typename FixedHash<T>::ConstructFromStringType;
// 	using ConstructFromPointerType = typename FixedHash<T>::ConstructFromPointerType;
// 	SecureFixedHash() = default;
// 	explicit SecureFixedHash(bytes const& _b, ConstructFromHashType _t = FixedHash<T>::FailIfDifferent): FixedHash<T>(_b, _t) {}
// 	explicit SecureFixedHash(bytesConstRef _b, ConstructFromHashType _t = FixedHash<T>::FailIfDifferent): FixedHash<T>(_b, _t) {}
// 	explicit SecureFixedHash(bytesSec const& _b, ConstructFromHashType _t = FixedHash<T>::FailIfDifferent): FixedHash<T>(_b.ref(), _t) {}
// 	template <unsigned M> explicit SecureFixedHash(FixedHash<M> const& _h, ConstructFromHashType _t = FixedHash<T>::AlignLeft): FixedHash<T>(_h, _t) {}
// 	template <unsigned M> explicit SecureFixedHash(SecureFixedHash<M> const& _h, ConstructFromHashType _t = FixedHash<T>::AlignLeft): FixedHash<T>(_h.makeInsecure(), _t) {}
// 	explicit SecureFixedHash(std::string const& _s, ConstructFromStringType _t = FixedHash<T>::FromHex, ConstructFromHashType _ht = FixedHash<T>::FailIfDifferent): FixedHash<T>(_s, _t, _ht) {}
// 	explicit SecureFixedHash(bytes const* _d, ConstructFromPointerType _t): FixedHash<T>(_d, _t) {}
// 	~SecureFixedHash() { ref().cleanse(); }

// 	SecureFixedHash<T>& operator=(SecureFixedHash<T> const& _c)
// 	{
// 		if (&_c == this)
// 			return *this;
// 		ref().cleanse();
// 		FixedHash<T>::operator=(static_cast<FixedHash<T> const&>(_c));
// 		return *this;
// 	}

// 	using FixedHash<T>::size;

// 	bytesSec asBytesSec() const { return bytesSec(ref()); }

// 	FixedHash<T> const& makeInsecure() const { return static_cast<FixedHash<T> const&>(*this); }
// 	FixedHash<T>& writable() { clear(); return static_cast<FixedHash<T>&>(*this); }

// 	using FixedHash<T>::operator bool;

// 	// The obvious comparison operators.
// 	bool operator==(SecureFixedHash const& _c) const { return static_cast<FixedHash<T> const&>(*this).operator==(static_cast<FixedHash<T> const&>(_c)); }
// 	bool operator!=(SecureFixedHash const& _c) const { return static_cast<FixedHash<T> const&>(*this).operator!=(static_cast<FixedHash<T> const&>(_c)); }
// 	bool operator<(SecureFixedHash const& _c) const { return static_cast<FixedHash<T> const&>(*this).operator<(static_cast<FixedHash<T> const&>(_c)); }
// 	bool operator>=(SecureFixedHash const& _c) const { return static_cast<FixedHash<T> const&>(*this).operator>=(static_cast<FixedHash<T> const&>(_c)); }
// 	bool operator<=(SecureFixedHash const& _c) const { return static_cast<FixedHash<T> const&>(*this).operator<=(static_cast<FixedHash<T> const&>(_c)); }
// 	bool operator>(SecureFixedHash const& _c) const { return static_cast<FixedHash<T> const&>(*this).operator>(static_cast<FixedHash<T> const&>(_c)); }

// 	using FixedHash<T>::operator==;
// 	using FixedHash<T>::operator!=;
// 	using FixedHash<T>::operator<;
// 	using FixedHash<T>::operator>=;
// 	using FixedHash<T>::operator<=;
// 	using FixedHash<T>::operator>;

// 	// The obvious binary operators.
// 	SecureFixedHash& operator^=(FixedHash<T> const& _c) { static_cast<FixedHash<T>&>(*this).operator^=(_c); return *this; }
// 	SecureFixedHash operator^(FixedHash<T> const& _c) const { return SecureFixedHash(*this) ^= _c; }
// 	SecureFixedHash& operator|=(FixedHash<T> const& _c) { static_cast<FixedHash<T>&>(*this).operator^=(_c); return *this; }
// 	SecureFixedHash operator|(FixedHash<T> const& _c) const { return SecureFixedHash(*this) |= _c; }
// 	SecureFixedHash& operator&=(FixedHash<T> const& _c) { static_cast<FixedHash<T>&>(*this).operator^=(_c); return *this; }
// 	SecureFixedHash operator&(FixedHash<T> const& _c) const { return SecureFixedHash(*this) &= _c; }

// 	SecureFixedHash& operator^=(SecureFixedHash const& _c) { static_cast<FixedHash<T>&>(*this).operator^=(static_cast<FixedHash<T> const&>(_c)); return *this; }
// 	SecureFixedHash operator^(SecureFixedHash const& _c) const { return SecureFixedHash(*this) ^= _c; }
// 	SecureFixedHash& operator|=(SecureFixedHash const& _c) { static_cast<FixedHash<T>&>(*this).operator^=(static_cast<FixedHash<T> const&>(_c)); return *this; }
// 	SecureFixedHash operator|(SecureFixedHash const& _c) const { return SecureFixedHash(*this) |= _c; }
// 	SecureFixedHash& operator&=(SecureFixedHash const& _c) { static_cast<FixedHash<T>&>(*this).operator^=(static_cast<FixedHash<T> const&>(_c)); return *this; }
// 	SecureFixedHash operator&(SecureFixedHash const& _c) const { return SecureFixedHash(*this) &= _c; }
// 	SecureFixedHash operator~() const { auto r = ~static_cast<FixedHash<T> const&>(*this); return static_cast<SecureFixedHash const&>(r); }

// 	using FixedHash<T>::abridged;
// 	using FixedHash<T>::abridgedMiddle;

// 	bytesConstRef ref() const { return FixedHash<T>::ref(); }
// 	byte const* data() const { return FixedHash<T>::data(); }

// 	static SecureFixedHash<T> random() { SecureFixedHash<T> ret; ret.randomize(s_fixedHashEngine); return ret; }
// 	using FixedHash<T>::firstBitSet;

// 	void clear() { ref().cleanse(); }
// };

// /// Fast equality operator for h256.
// template<> inline bool FixedHash<32>::operator==(FixedHash<32> const& _other) const
// {
// 	const uint64_t* hash1 = (const uint64_t*)data();
// 	const uint64_t* hash2 = (const uint64_t*)_other.data();
// 	return (hash1[0] == hash2[0]) && (hash1[1] == hash2[1]) && (hash1[2] == hash2[2]) && (hash1[3] == hash2[3]);
// }

// /// Fast std::hash compatible hash function object for h256.
// template<> inline size_t FixedHash<32>::hash::operator()(FixedHash<32> const& value) const
// {
// 	uint64_t const* data = reinterpret_cast<uint64_t const*>(value.data());
// 	return boost::hash_range(data, data + 4);
// }

// /// Stream I/O for the FixedHash class.
// template <unsigned N>
// inline std::ostream& operator<<(std::ostream& _out, FixedHash<N> const& _h)
// {
// 	_out << toHex(_h);
// 	return _out;
// }

// template <unsigned N>
// inline std::istream& operator>>(std::istream& _in, FixedHash<N>& o_h)
// {
// 	std::string s;
// 	_in >> s;
// 	o_h = FixedHash<N>(s, FixedHash<N>::FromHex, FixedHash<N>::AlignRight);
// 	return _in;
// }

// /// Stream I/O for the SecureFixedHash class.
// template <unsigned N>
// inline std::ostream& operator<<(std::ostream& _out, SecureFixedHash<N> const& _h)
// {
// 	_out << "SecureFixedHash#" << std::hex << typename FixedHash<N>::hash()(_h.makeInsecure()) << std::dec;
// 	return _out;
// }

// // Common types of FixedHash.
// using h2048 = FixedHash<256>;
// using h1024 = FixedHash<128>;
// using h520 = FixedHash<65>;
// using h512 = FixedHash<64>;
// using h256 = FixedHash<32>;
// using h160 = FixedHash<20>;
// using h128 = FixedHash<16>;
// using h64 = FixedHash<8>;
// using h512s = std::vector<h512>;
// using h256s = std::vector<h256>;
// using h160s = std::vector<h160>;
// using h256Set = std::set<h256>;
// using h160Set = std::set<h160>;
// using h256Hash = std::unordered_set<h256>;
// using h160Hash = std::unordered_set<h160>;

// /// Convert the given value into h160 (160-bit unsigned integer) using the right 20 bytes.
// inline h160 right160(h256 const& _t)
// {
// 	h160 ret;
// 	memcpy(ret.data(), _t.data() + 12, 20);
// 	return ret;
// }

// h128 fromUUID(std::string const& _uuid);

// std::string toUUID(h128 const& _uuid);

// inline std::string toString(h256s const& _bs)
// {
// 	std::ostringstream out;
// 	out << "[ ";
// 	for (h256 const& i: _bs)
// 		out << i.abridged() << ", ";
// 	out << "]";
// 	return out.str();
// }

// }

// namespace std
// {
// 	/// Forward std::hash<dev::FixedHash> to dev::FixedHash::hash.
// 	template<> struct hash<dev::h64>: dev::h64::hash {};
// 	template<> struct hash<dev::h128>: dev::h128::hash {};
// 	template<> struct hash<dev::h160>: dev::h160::hash {};
// 	template<> struct hash<dev::h256>: dev::h256::hash {};
// 	template<> struct hash<dev::h512>: dev::h512::hash {};
// }

}
