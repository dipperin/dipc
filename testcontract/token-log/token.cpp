#include "token.hpp"

EXPORT void TestToken::init(char* tokenName, char* sym, uint64_t supply){
    Address callerAddr = caller();
    std::string callerStr = callerAddr.toString();
    *owner = callerStr;
    *name = tokenName;
    *symbol = sym;
    *total_supply = supply;
    (*balance)[owner.get()] = supply;
    //prints(&owner.get()[0]);
    DIPC_EMIT_EVENT(Init, "", &owner.get()[0], supply);
}

EXPORT void TestToken::transfer(const char* to, uint64_t value){
    Address callerAddr = caller();
    std::string callStr = callerAddr.toString();
    //uint64_t originValue = balance.get()[callStr];
    //prints(&callStr[0]);
    //printui(originValue);
    //printui(value);
    bool result = (balance.get()[callStr] >= value);
    DipcAssert(result);

    std::string toStr = CharToAddress2Str(to);
    DipcAssert(balance.get()[toStr] + value >= balance.get()[toStr]);

    (*balance)[callStr] = balance.get()[callStr] -  value;
    (*balance)[toStr] = balance.get()[toStr] + value;
    DIPC_EMIT_EVENT(Transfer, &(callStr[0]), to, value);
}

EXPORT void TestToken::approve(const char* spender, uint64_t value){
    Address callerAddr = caller();
    std::string spenderStr = CharToAddress2Str(spender);
   
    uint64_t total = allowance.get()[callerAddr.toString()+spenderStr] + value;
    (*allowance)[callerAddr.toString()+spenderStr] = total;
    DIPC_EMIT_EVENT(Approval, &(callerAddr.toString()[0]), spender, value);
}

EXPORT void TestToken::transferFrom(const char* from, const char* to, uint64_t value){
    Address callerAddr = caller();
    std::string fromStr = CharToAddress2Str(from);
    std::string toStr = CharToAddress2Str(to);

    DipcAssert(balance.get()[fromStr] >= value);
    DipcAssert(balance.get()[toStr] + value >= balance.get()[toStr]);
    DipcAssert(allowance.get()[fromStr+callerAddr.toString()] >= value);

    (*balance)[toStr] = balance.get()[toStr] + value;
    (*balance)[fromStr] = balance.get()[fromStr] - value;
    (*allowance)[fromStr +callerAddr.toString()] = allowance.get()[fromStr+callerAddr.toString()] - value; 
    DIPC_EMIT_EVENT(Transfer, from, to, value);
}

EXPORT void TestToken::burn(uint64_t value){
    Address callerAddr = caller();
    DipcAssert(balance.get()[callerAddr.toString()] >= value);
    DipcAssert(balance.get()[owner.get()] + value >= balance.get()[owner.get()]);
    //prints("burn======");
    //prints(&owner.get()[0]);
    //printui(balance.get()[owner.get()]);
    (*balance)[callerAddr.toString()] -= value;
    (*balance)[owner.get()] += value;
    //printui(balance.get()[owner.get()]);
    DIPC_EMIT_EVENT(Transfer, &(callerAddr.toString()[0]), &(owner.get()[0]), value);
}

CONSTANT uint64_t TestToken::getBalance(const char* own){
    //prints(own);
    std::string ownerStr = CharToAddress2Str(own);
    uint64_t ba =  balance.get()[ownerStr];
    //printui(ba);
    return ba;
}

CONSTANT uint64_t TestToken::getApproveBalance(const char* from, const char* approved){
    //prints("getApproveBalance");
    //prints(from);
   // prints(approved);
    std::string fromStr = CharToAddress2Str(from);
    std::string approvedStr = CharToAddress2Str(approved);
    uint64_t re = allowance.get()[fromStr+approvedStr];
    //printui(re);
    return re;
}