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
    EXPORT void init(const char* tokenName,const char* symbol, uint64_t supply);
    PAYABLE void transfer(const char* to, uint64_t value);
    EXPORT void approve(const char* spender, uint64_t value);
    EXPORT void transferFrom(const char* from, const char* to, uint64_t value);
    CONSTANT uint64_t getBalance(const char* own);
    CONSTANT uint64_t getApproveBalance(const char* from, const char* approved);
    EXPORT bool burn(int128_t _value);

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
        *name = _name;
    }

private: 
    String<na> name ;
    String<sy> symbol;
    uint8_t decimals = 6;
    Map<bal, std::string, uint64_t >  balance;
    Map<allow, std::string, uint64_t> allowance;
    Uint64<tmp> total_supply;
    bool stopped = false;
    String<ow> owner;
    
    inline void isOwner(){
        Address callerAddr = caller();
        std::string callerStr = callerAddr.toString();
        DipcAssert(owner.get() == callerStr);
    }
};
DIPC_EVENT(Tranfer, const char*, const char*, uint64_t);
DIPC_EVENT(Approval, const char*, const char*, uint64_t);
DIPC_EVENT(GetBalance, const char*, const char*, uint64_t);
