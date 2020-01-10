#include "token.hpp"

EXPORT void TestToken::init(const char* tokenName, const char* sym, uint64_t supply){

    Address callerAddr = caller();
    std::string callerStr = callerAddr.toString();
    *owner = callerStr;
    *name = tokenName;
    *symbol = sym;
    *total_supply = supply;
    (*balance)[owner.get()] = supply;
    DIPC_EMIT_EVENT(Init,&name.get()[0], &symbol.get()[0], &owner.get()[0], supply);
}

EXPORT void TestToken::approve(const char* spender, uint64_t value){
    Address callerAddr = caller();
   
    uint64_t total = allowance.get()[callerAddr.toString()+spender] + value;
    (*allowance)[callerAddr.toString()+spender] = total;
    DIPC_EMIT_EVENT(Approval, &(callerAddr.toString()[0]), spender, value);
}

PAYABLE void TestToken::transfer(const char* to, uint64_t value){
    Address callerAddr = caller();
    std::string callStr = callerAddr.toString();
    bool result = (balance.get()[callStr] >= value);
    DipcAssert(result);

    std::string toStr = CharToAddress2Str(to);
    DipcAssert(balance.get()[toStr] + value >= balance.get()[toStr]);

    (*balance)[callStr] = balance.get()[callStr] -  value;
    (*balance)[toStr] = balance.get()[toStr] + value;
    DIPC_EMIT_EVENT(Tranfer, &(callStr[0]), to, value);
}



EXPORT void TestToken::transferFrom(const char* from, const char* to, uint64_t value){
    Address callerAddr = caller();

    DipcAssert(balance.get()[from] >= value);
    DipcAssert(balance.get()[to] + value >= balance.get()[to]);
    DipcAssert(allowance.get()[from+callerAddr.toString()] >= value);

    (*balance)[to] = balance.get()[to] + value;
    (*balance)[from] = balance.get()[from] - value;
    (*allowance)[from +callerAddr.toString()] = allowance.get()[from+callerAddr.toString()] - value; 
    DIPC_EMIT_EVENT(TranferFrom, from, to, value);
}

EXPORT bool TestToken::burn(uint64_t value){
    Address callerAddr = caller();
    DipcAssert(balance.get()[callerAddr.toString()] >= value);
    DipcAssert(balance.get()[owner.get()] + value >= balance.get()[owner.get()]);
    (*balance)[callerAddr.toString()] -= value;
    (*balance)[owner.get()] += value;
    DIPC_EMIT_EVENT(Burn, &(callerAddr.toString()[0]), value);
    DIPC_EMIT_EVENT(Tranfer, &(callerAddr.toString()[0]), &(owner.get()[0]), value);
    return true;
}

CONSTANT uint64_t TestToken::getBalance(const char* own){
    uint64_t ba =  balance.get()[own];
    return ba;
}

CONSTANT uint64_t TestToken::getApproveBalance(const char* from, const char* approved){
    std::string fromStr = from;
    uint64_t re = allowance.get()[fromStr+approved];
    return re;
}