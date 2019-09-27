#pragma once
#include <dipc/dipc.hpp>
#include <string>

using namespace dipc;

char tmp[7] = "supply";
char bal[8] = "balance";
char na[5] = "dipc";
char allow[10] = "allowance";
char ow[4] = "own";
char sy[7] = "symbol";
class TestToken : public Contract {
public: 
    EXPORT void init(char* tokenName, char* symbol, uint64_t supply);
    EXPORT void transfer(const char* to, uint64_t value);
    EXPORT void approve(const char* spender, uint64_t value);
    EXPORT void transferFrom(const char* from, const char* to, uint64_t value);
    CONSTANT uint64_t getBalance(const char* own);
    CONSTANT uint64_t getApproveBalance(const char* from, const char* approved);
    EXPORT void burn(uint64_t _value);

    void stop(){
        isOwner();
        stopped = true;    
    }
    void start() {
        isOwner();
        stopped = false;
    }
    void setName(const char* _name){
        isOwner();
        //当使用非指针类型对存储型变量赋值时，会报unreachable错误
        *name = _name;
    }

private: 
    String<na> name ;
    String<sy> symbol;
    uint8_t decimals = 6;
    Map<bal, std::string, uint64_t >  balance;
    //         存放授权地址    存放授权金额
    Map<allow, std::string, uint64_t> allowance;
    Uint64<tmp> total_supply;
    bool stopped = false;
    // 当owner和name使用同一个char*标记na时，会导致数据出错
    String<ow> owner;
    
    inline void isOwner(){
        Address callerAddr = caller();
        std::string callerStr = callerAddr.toString();
        DipcAssert(owner.get() == callerStr);
    }
};

DIPC_EVENT(Transfer, const char*, const char*, uint64_t);
DIPC_EVENT(Approval, const char*, const char*, uint64_t);
DIPC_EVENT(Init, const char*, const char*, uint64_t);

