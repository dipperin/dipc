//
// Created by zhou.yang on 2018/10/19.
//

#pragma once

#include "fixedhash.hpp"


extern "C" {
    int64_t gasPrice();
    void blockHash(int64_t num,  uint8_t[32]);
    uint64_t number();
    uint64_t gasLimit();
    int64_t timestamp();
    void coinbase(uint8_t hash[22]);
    void balance(const uint8_t *src, size_t srcLen, uint8_t amount[32]);
    void origin(uint8_t hash[22]);
    void caller(uint8_t hash[22]);
    void callValue(uint8_t val[32]);
    uint64_t callValueUDIP();
    void address(uint8_t hash[22]);
    //void address2(uint8_t hash[47]);
    void sha3(const uint8_t *src, size_t srcLen, uint8_t *dest, size_t destLen);
    int64_t getCallerNonce();
	int64_t callTransfer(const uint8_t* to, size_t toLen, uint8_t amount[32]);
    int64_t callTransferUDIP(const uint8_t* to, size_t toLen, int64_t balance);
    void getSignerAddress(const uint8_t* sha3Data, size_t sha3DataLen, const uint8_t *signature, size_t signatureLen, uint8_t addr[22]);
}

namespace dipc {
    /**
     * @brief Get block hash
     * 
     * @param number Block height
     * @return h256 Block hash
     */
    inline h256 blockHash(int64_t number){
        byte hash[32];
        ::blockHash(number, hash);
        return h256(hash, sizeof(hash));
    }
    

    /**
     * @brief Current block miner’s address
     * 
     * @return h160 
     */
    // inline h160 coinbase(){
    //     byte hash[20];
    //     ::coinbase(hash);
    //     return h160(hash, sizeof(hash));
    // }

    inline Address coinbase(){
        byte hash[22];
        ::coinbase(hash);
        return Address(hash, sizeof(hash));
    }

    /**
     * @brief Get the balance
     * 
     * @return u256 balance
     */
    inline u256 balance(const Address& addr) {
        byte amount[32];
        ::balance(addr.data(), addr.size(), amount);
        return fromBigEndian<u256>(amount);
    }

    /**
     * @brief Sender of the transaction (full call chain)
     * 
     * @return h160 
     */
    // inline h160 origin(){
    //     byte hash[20];
    //     ::origin(hash);
    //     return h160(hash, sizeof(hash));
    // }

    inline Address origin(){
        byte hash[22];
        ::origin(hash);
        return Address(hash, sizeof(hash));
    }

    /**
     * @brief Caller address
     * 
     * @return h160 
     */
    // inline h160 caller(){
    //     byte hash[20];
    //     ::caller(hash);
    //     return h160(hash, sizeof(hash));
    // }

    inline Address caller(){
        byte hash[22];
        ::caller(hash);
        return Address(hash, sizeof(hash));
    }

    /**
     * @brief Number of wei sent with the message
     * 
     * @return u256 
     */
    inline u256 callValue() {
        byte val[32];
        ::callValue(val);
        return fromBigEndian<u256>(val);
    }

    inline uint64_t callValueUDIP(){
        return ::callValueUDIP();
    }

    /**
     * @brief Contract address
     * 
     * @return h160 
     */

    inline Address address(){
        byte hash[22];
        ::address(hash);
        return Address(hash, sizeof(hash));
    }

    /**
     * @brief Compute the Keccak-256 hash of the input
     * 
     * @param data Binary data
     * @return h256 
     */
    inline h256 sha3(bytes & data) {
        byte hash[32];
        ::sha3(data.data(), data.size(), hash, sizeof(hash));
        return h256(hash, sizeof(hash));
    }

    /**
     * @brief Compute the Keccak-256 hash of the input
     * 
     * @param data String
     * @return h256 
     */
    inline h256 sha3(const std::string &data) {
        byte hash[32];
        ::sha3((const byte*)data.data(), data.length(), hash, sizeof(hash));
        return h256(hash, sizeof(hash));
    }


    /**
     * @brief Compute the Keccak-256 hash of the input
     * 
     * @param data Byte pointer
     * @param len Length
     * @return h256 
     */
    inline h256 sha3(const byte *data, size_t len) {
        byte hash[32];
        ::sha3(data, len, hash, sizeof(hash));
        return h256(hash, sizeof(hash));
    }

    /**
     * @brief Send given amount of WU to Address
     * 
     * @param to Destination address
     * @param amount Amount
     * @return int64_t 0 success non-zero failure
     */
    inline int64_t callTransfer(const Address& to, u256 amount) {
        bytes bs(32);
        toBigEndian(amount, bs);
        return ::callTransfer(to.data(), to.size(), bs.data());
    }

        /**
     * @brief Send given amount of WU to Address
     * 
     * @param to Destination address
     * @param amount Amount
     * @return int64_t 0 success non-zero failure
     */
    inline int64_t callTransferUDIP(const Address& to, uint64_t amount) {
        return ::callTransferUDIP(to.data(),to.size(), amount);
    }

    /**
     * @brief  From the signature and sha3Data recover the signer
     * 
     * @param signature signed data
     * @param sha3Data  message crypto by the sha2 function
     * @return Address the singer address
     */
    inline Address getSignerAddress(const h256 sha3Data, std::string signature){
        byte addr[22];
        ::getSignerAddress((const byte*)sha3Data.data(), sha3Data.size(), (const byte*) signature.data(), signature.size(), addr );
        return Address(addr, sizeof(addr));
    }
    /**
     *  @brief Get the current Block header timestamp
     *  @return the current Block header timestamp unixnano value
     */
    inline int64_t timestamp(){
        return ::timestamp();
    }

}


