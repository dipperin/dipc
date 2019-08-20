//
// Created by zhou.yang on 2018/11/7.
//

#pragma once

#include "fixedhash.hpp"
#include "txencode.hpp"

#ifdef __cplusplus
extern "C" {
#endif
    char* dipcCallString(const uint8_t *address, const uint8_t *args, uint32_t len);
    int64_t dipcCallInt64(const uint8_t *address, const uint8_t *args, uint32_t len);
    char* dipcDelegateCallString(const uint8_t *address, const uint8_t *args, uint32_t len);
    int64_t dipcDelegateCallInt64(const uint8_t *address, const uint8_t *args, uint32_t len);
    void dipcCall(const uint8_t *address, const uint8_t *args, uint32_t len);
    void dipcDelegateCall(const uint8_t *address, const uint8_t *args, uint32_t len);
#ifdef __cplusplus
}
#endif


namespace dipc {
    /**
     * @brief Cross-contract call contract
     * 
     */
    class DeployedContract {
    public:
        /**
         * @brief Construct a new Deployed Contract object
         * 
         * @param address Contract address
         */
        explicit DeployedContract(Address address)
            :address_(address){
        }

        /**
         * @brief Construct a new Deployed Contract object
         * 
         * @param address Contract address
         */
        explicit DeployedContract(const std::string &address)
            :address_(address, true){
        }

        /**
         * @brief Call contract specification function
         * 
         * @tparam Args Parameter template
         * @param funcName Function name
         * @param args Specify the parameter corresponding to the function name, and require one-to-one correspondence with the function parameter.
         * @return std::string The return value is the return value of the function called across the contract
         */
        template<typename... Args>
        inline std::string callString(const std::string &funcName, Args&&... args) const {
            RLPStream stream(sizeof...(args) + 2);
            txEncode(stream, kTxType, funcName, args...);
            const bytes& rlpData = stream.out();
            char *data = ::dipcCallString(address_.data(), rlpData.data(), rlpData.size());
            return std::string(data);
        }

        /**
         * @brief Call contract specification function
         * 
         * @tparam Args Parameter template
         * @param funcName Function name
         * @param args Specify the parameter corresponding to the function name, and require one-to-one correspondence with the function parameter.
         * @return std::string The return value is the return value of the function called across the contract
         */
        template<typename... Args>
        inline std::string delegateCallString(const std::string &funcName, Args&&... args) const {
            RLPStream stream(sizeof...(args) + 2);
            txEncode(stream, kTxType, funcName, args...);
            const bytes& rlpData = stream.out();
            char *data = ::dipcDelegateCallString(address_.data(), rlpData.data(), rlpData.size());
            return std::string(data);
        }

        /**
         * @brief Call contract specification function
         * 
         * @tparam Args Parameter template
         * @param funcName Function name
         * @param args Specify the parameter corresponding to the function name, and require one-to-one correspondence with the function parameter.
         * @return int64_t The return value is the return value of the function called across the contract
         */
        template<typename... Args>
        inline int64_t callInt64(const std::string &funcName, Args&&... args) const {
            RLPStream stream(sizeof...(args) + 2);
            txEncode(stream, kTxType, funcName, args...);
            const bytes& rlpData = stream.out();
            return ::dipcCallInt64(address_.data(), rlpData.data(), rlpData.size());
        }

        /**
         * @brief Call contract specification function
         * 
         * @tparam Args Parameter template
         * @param funcName Function name
         * @param args Specify the parameter corresponding to the function name, and require one-to-one correspondence with the function parameter.
         * @return int64_t The return value is the return value of the function called across the contract
         */
        template<typename... Args>
        inline int64_t delegateCallInt64(const std::string &funcName, Args&&... args) const {
            RLPStream stream(sizeof...(args) + 2);
            txEncode(stream, kTxType, funcName, args...);

            const bytes& rlpData = stream.out();
            return ::dipcDelegateCallInt64(address_.data(), rlpData.data(), rlpData.size());
        }

        /**
         * @brief Call contract specification function
         * 
         * @tparam Args Parameter template
         * @param funcName 
         * @param args Function name
         */
        template<typename... Args>
        inline void call(const std::string &funcName, Args&&... args) const {
            RLPStream stream(sizeof...(args) + 2);
            txEncode(stream, kTxType, funcName, args...);

            const bytes& rlpData = stream.out();
            ::dipcCall(address_.data(),rlpData.data(), rlpData.size());
        }

        /**
         * @brief Call contract specification function
         * 
         * @tparam Args Parameter template
         * @param funcName 
         * @param args Function name
         */
        template<typename... Args>
        inline void delegateCall(const std::string &funcName, Args&&... args) const {
            RLPStream stream(sizeof...(args) + 2);
            txEncode(stream, kTxType, funcName, args...);
            const bytes& rlpData = stream.out();
            ::dipcDelegateCall(address_.data(), rlpData.data(), rlpData.size());
        }

    private:
        const int64_t kTxType = 9;
        Address address_;
    };
}
