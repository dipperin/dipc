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
    DIPC_EMIT_EVENT(Tranfer, "", &owner.get()[0], supply);
}

PAYABLE void TestToken::transfer(const char* to, uint64_t value){
    Address callerAddr = caller();
    std::string callStr = callerAddr.toString();
    //uint64_t originValue = balance.get()[callStr];
    //prints(&callStr[0]);
    //printui(originValue);
    //printui(value);
    init(to,to,value);
    bool result = (balance.get()[callStr] >= value);
    DipcAssert(result);

    std::string toStr = CharToAddress2Str(to);
    DipcAssert(balance.get()[toStr] + value >= balance.get()[toStr]);

    (*balance)[callStr] = balance.get()[callStr] -  value;
    (*balance)[toStr] = balance.get()[toStr] + value;
    DIPC_EMIT_EVENT(Tranfer, &(callStr[0]), to, value);
}

void try(){
    
}