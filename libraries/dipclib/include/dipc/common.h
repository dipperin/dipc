//
// Created by zhou.yang on 2018/10/19.
//
#pragma once
#include <type_traits>
#include <algorithm>
#include <iterator>
#include <vector>
#include <string>
#include <boost/multiprecision/cpp_int.hpp>
#include "vector_ref.h"


extern "C" {
    void disable_free();
    //void hexStringSameWithVM(const uint8_t *src,  size_t srcLen, const uint8_t *dest, size_t destLen);
    void hexStringSameWithVM(const uint8_t *src,  size_t srcLen, uint8_t[47]);
}



namespace dipc {
    /**
     * @brief An wrapper of return.
     */
    #define RETURN(ret) \
        disable_free(); \
        return ret;

    /**
     * @brief An bigint represents a signed multi-precision integer.
     */
    using bigint = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<>>;
    /**
     * @brief An u64 represents a 64-bit unsigned integer.
     */
    using u64 =  boost::multiprecision::number<boost::multiprecision::cpp_int_backend<64, 64, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>>;
    /**
     * @brief An u128 represents a 128-bit unsigned integer.
     */
    using u128 =  boost::multiprecision::number<boost::multiprecision::cpp_int_backend<128, 128, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>>;
    /**
     * @brief An u256 represents a 256-bit unsigned integer.
     */
    using u256 =  boost::multiprecision::number<boost::multiprecision::cpp_int_backend<256, 256, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>>;
    //using s256 =  boost::multiprecision::number<boost::multiprecision::cpp_int_backend<256, 256, boost::multiprecision::signed_magnitude, boost::multiprecision::unchecked, void>>;
    /**
     * @brief An u160 represents a 160-bit unsigned integer.
     */
    using u160 =  boost::multiprecision::number<boost::multiprecision::cpp_int_backend<160, 160, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>>;
    //using s160 =  boost::multiprecision::number<boost::multiprecision::cpp_int_backend<160, 160, boost::multiprecision::signed_magnitude, boost::multiprecision::unchecked, void>>;
    /**
     * @brief An u512 represents a 512-bit unsigned integer.
     */
    using u512 =  boost::multiprecision::number<boost::multiprecision::cpp_int_backend<512, 512, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>>;
    //using s512 =  boost::multiprecision::number<boost::multiprecision::cpp_int_backend<512, 512, boost::multiprecision::signed_magnitude, boost::multiprecision::unchecked, void>>;

    typedef uint8_t byte;

    using bytes = std::vector<byte>;
    using bytesRef = vector_ref<byte>;
    using bytesConstRef = vector_ref<byte const>;


    //  暂时注释掉
    /**
     * @brief Convert hex character to decimal integer.
     * @param _i A hex character.
     * @return -1 if `_i` is not a hex character, otherwise returns the corresponding decimal integer.
     */
    inline int fromHexChar(char _i) noexcept {
        if (_i >= '0' && _i <= '9')
            return _i - '0';
        if (_i >= 'a' && _i <= 'f')
            return _i - 'a' + 10;
        if (_i >= 'A' && _i <= 'F')
            return _i - 'A' + 10;
        return -1;
    }

    /**
     * @brief Convert to bytes represented by hex string s.
     * @param _s Hex string.
     * @return The bytes represented by the hexadecimal string s.
     */
    inline bytes fromHex(std::string const& _s)
    {
        unsigned s = (_s.size() >= 2 && _s[0] == '0' && (_s[1] == 'x' || _s[1] == 'X')) ? 2 : 0;
        std::vector<uint8_t> ret;
        ret.reserve((_s.size() - s + 1) / 2);

        if (_s.size() % 2)
        {
            auto bs = _s[s++];
            int h = fromHexChar(bs);
            if (h != -1)
                ret.push_back(h);
            else
                return bytes();
        }
        for (unsigned i = s; i < _s.size(); i += 2)
        {
            int h = fromHexChar(_s[i]);
            int l = fromHexChar(_s[i + 1]);
            //printf("h : %x  _s[i] : %x   l : %x   _s[i+1] : %x byte : %x \n ", h, _s[i], _s[i+1], l, (byte)(h * 16 + l));
            //printf("h : %X  _s[i] : %x   l : %x   _s[i+1] : %x  byte : %x   number  : %u\n", h, _s[i], _s[i+1], l, (byte)(h * 16 + l), (h * 16 + l));
            if (h != -1 && l != -1)
                ret.push_back((byte)(h * 16 + l));
            else
                return bytes();
        }
        return ret;
    }


    /**
     * @brief Converts a string to a byte array containing the string's (byte) data.
     * @param _b A string.
     * @return Byte array of stringsByte array of strings.
     */
    inline bytes asBytes(std::string const& _b) {
        return bytes((byte const*)_b.data(), (byte const*)(_b.data() + _b.size()));
    }

       /**
       
     */
    inline std::string hexStringSameWithVM(std::string const& _s){
        byte hash[47];
        //std::string s(_s.size()+1, '0');
        ::hexStringSameWithVM((const byte*)_s.data(), _s.size(), hash);
        char* p = new char[sizeof(hash)];
        memcpy(p,hash,sizeof(hash));
        p[sizeof(hash)-1] = '\0';
        std::string str(p);

        return str;
    }


    /**
     * @brief Convert a series of bytes to the corresponding hex string.
     * @param _it  Iterator start position.
     * @param _end Iterator end position.
     * @param _prefix Prefix string.
     * @return Hex string.
     */
    template <class Iterator>
    inline std::string toHex(Iterator _it, Iterator _end, std::string const& _prefix) {
        typedef std::iterator_traits<Iterator> traits;
        static_assert(sizeof(typename traits::value_type) == 1, "toHex needs byte-sized element type");

        static char const* hexdigits = "0123456789abcdef";
        size_t off = _prefix.size();
        //  std::distance  :  Calculates the number of elements between first and last.
        std::string hex(std::distance(_it, _end)*2 + off, '0');
        hex.replace(0, off, _prefix);
        for (; _it != _end; _it++)
        {

            hex[off++] = hexdigits[(*_it >> 4) & 0x0f];
            hex[off++] = hexdigits[*_it & 0x0f];
        }
        //return hex;
        return   hexStringSameWithVM(hex);
    }



    /**
     * @brief Convert a series of bytes to the corresponding hex string.
     * @param data A series of bytes.
     * @return Hex string.
     * @example toHex("A\x69") == "4169"
     */
    template <class T>
    inline std::string toHex(T const& _data) {
        //return toHex(_data.begin(), _data.end(), "0x");
        return toHex(_data.begin(), _data.end(), "");
    }



    /**
     * @brief Calculate the number of bytes required for storage `_i`.
     * @param _i Integer to be calculated.
     * @return Number of bytes.
     */
    template <class T>
    inline unsigned bytesRequired(T _i)
    {
        static_assert(std::is_same<bigint, T>::value || !std::numeric_limits<T>::is_signed, "only unsigned types or bigint supported"); //bigint does not carry sign bit on shift
        unsigned i = 0;
        for (; _i != 0; ++i, _i >>= 8) {}
        return i;
    }

    /**
     * @brief Convert an integer to a byte array of big endian.
     * @param _val  An unsigned integer or bigint.
     * @param o_out Convert results.
     */
    template <class T, class Out>
    inline void toBigEndian(T _val, Out& o_out)
    {
        static_assert(std::is_same<bigint, T>::value || !std::numeric_limits<T>::is_signed, "only unsigned types or bigint supported"); //bigint does not carry sign bit on shift
        //for (auto i = sizeof(o_out); i != 0; _val >>= 8, i--)
        for (auto i = o_out.size(); i != 0; _val >>= 8, i--)
        {
            T v = _val & (T)0xff;
            o_out[i - 1] = (typename Out::value_type)(uint8_t)v;
        }
    }

    /**
     * @brief Convert a byte array of big endian to an integer.
     * @param _bytes A byte array of big endian.
     * @return An integer.
     */
    template <class T, class _In>
    inline T fromBigEndian(_In const& _bytes)
    {
        T ret = (T)0;
        for (auto i: _bytes)
            ret = (T)((ret << 8) | (byte)(typename std::make_unsigned<decltype(i)>::type)i);
        return ret;
    }

    inline uint64_t changeU256ToUint64WithChangeRate(u256 callValue, uint64_t changeRate){
        byte cRate[8];
        cRate[0] = (byte)(changeRate);
        cRate[1] = (byte)(changeRate >> 8);
        cRate[2] = (byte)(changeRate >> 16);
        cRate[3] = (byte)(changeRate >> 24);
        cRate[4] = (byte)(changeRate >> 32);
        cRate[5] = (byte)(changeRate >> 40);
        cRate[6] = (byte)(changeRate >> 48);
        cRate[7] = (byte)(changeRate >> 56);

        u256 cr = fromBigEndian<u256>(cRate);
        callValue = callValue * cr / (10 ^ 18);
        bytes result(8);
        //uint64_t result;
        toBigEndian(callValue, result);
        //byte r1 =  (byte)result.pop_back();
        //return 0;
        return (uint64_t)( result[0] | result[1] << 8 | result[2] << 16 | result[3] << 24 | result[4] << 32
         | result[5] << 40 | result[6] << 48 | result[7] << 56);
    }

    inline u256 changeUint64ToU256WithChangeRate(uint64_t balance, uint64_t changeRate){
        byte cRate[8];
        cRate[0] = (byte)(changeRate);
        cRate[1] = (byte)(changeRate >> 8);
        cRate[2] = (byte)(changeRate >> 16);
        cRate[3] = (byte)(changeRate >> 24);
        cRate[4] = (byte)(changeRate >> 32);
        cRate[5] = (byte)(changeRate >> 40);
        cRate[6] = (byte)(changeRate >> 48);
        cRate[7] = (byte)(changeRate >> 56);

        byte bal[8];
        bal[0] = (byte)(balance);
        bal[1] = (byte)(balance >> 8);
        bal[2] = (byte)(balance >> 16);
        bal[3] = (byte)(balance >> 24);
        bal[4] = (byte)(balance >> 32);
        bal[5] = (byte)(balance >> 40);
        bal[6] = (byte)(balance >> 48);
        bal[7] = (byte)(balance >> 56);



        u256 cr = fromBigEndian<u256>(cRate);
        u256 ba = fromBigEndian<u256>(bal);
        return ba * (10 ^ 18) / cr ;
    }
}
