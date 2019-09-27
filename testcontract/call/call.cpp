#include "call.hpp"

EXPORT void call::init(char* tokenName, char* sym, uint64_t supply){
    Address originAddr = origin();
    std::string originStr = originAddr.toString();
    *owner = originStr;
    *name = tokenName;
    *symbol = sym;
    *total_supply = supply;
    (*balance)[owner.get()] = supply;
    //prints(&owner.get()[0]);
    DIPC_EMIT_EVENT(CallTransfer, "", &owner.get()[0], supply);
}

PAYABLE void call::callInit(const char* contractAddr, const char* tokenName, const char* sym, uint64_t supply) {
  Address callerAddr = caller();
  DeployedContract contract(contractAddr);
  contract.call("init", tokenName, sym, supply);
  DIPC_EMIT_EVENT(CallTransfer, &(callerAddr.toString()[0]), tokenName, supply);
}

PAYABLE void call::callTokenTransfer(const char* contractAddr, const char* to, uint64_t value) {
  Address callerAddr = caller();
  DeployedContract contract(contractAddr);
  contract.call("transfer", to, value);
  DIPC_EMIT_EVENT(CallTransfer, &(callerAddr.toString()[0]), to, value);
}

PAYABLE void call::delegateCallTokenTransfer(const char* contractAddr, const char* to, uint64_t value) {
  Address callerAddr = caller();
  DeployedContract contract(contractAddr);
  contract.delegateCall("transfer", to, value);
  DIPC_EMIT_EVENT(CallTransfer, &(callerAddr.toString()[0]), to, value);
}

PAYABLE bool call::transfer(const char* to, uint64_t value){
    isZeroAddress(to);
    Address callerAddr = caller();
    std::string callStr = callerAddr.toString();
    std::string toStr = CharToAddress2Str(to);

    // check uint64 underflow and over flow
    DipcAssert(balance.get()[callStr] >= value);
    DipcAssert(balance.get()[toStr] + value > balance.get()[toStr]);

    (*balance)[callStr] -=  value;
    (*balance)[toStr] += value;
    return true;
}

CONSTANT uint64_t call::callGetBalance(const char* contractAddr, const char* own) {
  DeployedContract contract(contractAddr);
  uint64_t ba = contract.callInt64("getBalance", own);
  return ba;
}

CONSTANT uint64_t call::delegateCallGetBalance(const char* contractAddr, const char* own) {
  DeployedContract contract(contractAddr);
  uint64_t ba = contract.delegateCallInt64("getBalance", own);
  return ba;
}

CONSTANT uint64_t call::getBalance(const char* own) {
    std::string ownerStr = CharToAddress2Str(own);
    uint64_t ba =  balance.get()[ownerStr];
    return ba;
}