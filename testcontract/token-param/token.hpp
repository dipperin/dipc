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
    EXPORT void init(const std::string tokenName,const std::string symbol, uint64_t supply);
    PAYABLE void transfer(const std::string to, uint64_t value);
    EXPORT void approve(const std::string spender, uint64_t value);
    WITHDRAW void transferFrom(const std::string from, const std::string to, uint64_t value);
    PAYABLE uint64_t getBalance(const std::string own);
    CONSTANT uint64_t getApproveBalance(const std::string from, const std::string approved);
    void burn(uint64_t _value);

    void stop(){
        isOwner();
        stopped = true;    
    }
    void start() {
        isOwner();
        stopped = false;
    }
    void setName(const std::string _name){
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
private: inline void approve_inline(const std::string spender, uint64_t value);
};
// 没有加这个宏  导致编译wasm的时候没通过也没报错  待优化
// DIPC_ABI(TestToken, init);
// DIPC_ABI(TestToken, transfer);
// DIPC_ABI(TestToken, start);
// DIPC_ABI(TestToken, stop);
// DIPC_ABI(TestToken, setName);
// DIPC_ABI(TestToken, transferFrom);
// DIPC_ABI(TestToken, getBalance);
// DIPC_ABI(TestToken, getApproveBalance);
// DIPC_ABI(TestToken, approve);
// DIPC_ABI(TestToken, burn);
DIPC_EVENT(Tranfer, const std::string, const std::string, uint64_t);
DIPC_EVENT(Approval, const std::string, const std::string, uint64_t);
DIPC_EVENT(GetBalance, const std::string, const std::string, uint64_t);
