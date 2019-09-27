#include "token.hpp"
#include <stdio.h>

void TestToken::init(char *tokenName, char *sym, uint64_t supply)
{
    //isOwner();
    //std::string _tokenName = tokenName;
    //std::string _sym = sym;
    std::string ownerStr = owner.toString();
    // 赋值只能使用指针操作，否则虚拟机调用时会报错
    *name = tokenName;
    *symbol = sym;
    *total_supply = supply;
    (*balance)[ownerStr] = supply;
    prints(&(ownerStr[0]));
    //printui(supply);
    DIPC_EMIT_EVENT(Tranfer, "", &(ownerStr[0]), supply);
}

void TestToken::transfer(const char *to, uint64_t value)
{
    Address callerAddr = caller();
    std::string callStr = callerAddr.toString();
    std::string ownStr = owner.toString();
    prints(&callStr[0]);
    prints(&ownStr[0]);
    uint64_t originValue = balance.get()[callStr];
    printui(originValue);
    printui(value);
    bool result = (originValue >= value);
    DipcAssert(result);
    std::string toStr = CharToAddress2Str(to);
    DipcAssert(balance.get()[toStr] + value >= balance.get()[toStr]);
    (*balance)[callStr] = balance.get()[callStr] -  value;
    (*balance)[toStr] = balance.get()[toStr] + value;
    DIPC_EMIT_EVENT(Tranfer, &(callStr[0]), to, value);
}
void TestToken::transferFrom(const char *from, const char *to, uint64_t value)
{
    Address callerAddr = caller();

    std::string fromStr = CharToAddress2Str(from);
    std::string toStr = CharToAddress2Str(to);
    DipcAssert(balance.get()[fromStr] >= value);
    DipcAssert(balance.get()[toStr] + value >= balance.get()[toStr]);
    DipcAssert(balance.get()[fromStr+callerAddr.toString()] >= value);
    uint64_t toValue = balance.get()[toStr] + value;
    uint64_t fromValue = balance.get()[fromStr] - value;
    prints("transferFrom");
    printui(toValue);
    printui(fromValue);
    (*balance)[toStr] = toValue;
    (*balance)[fromStr] = fromValue;
    (*balance)[fromStr +callerAddr.toString()] = balance.get()[fromStr+callerAddr.toString()] - value; 
    DIPC_EMIT_EVENT(Tranfer, from, to, value);
}
void TestToken::approve(const char *spender, uint64_t value)
{
    Address callerAddr = caller();
    std::string spenderStr = CharToAddress2Str(spender);
    DipcAssert(value == 0 || balance.get()[callerAddr.toString()+spenderStr] == 0);
    (*balance)[callerAddr.toString()+spenderStr] = value;
    DIPC_EMIT_EVENT(Approval, &(callerAddr.toString()[0]), spender, value);
}
void TestToken::burn(uint64_t value)
{
    Address callerAddr = caller();
    DipcAssert(balance.get()[callerAddr.toString()] >= value);
    (*balance)[callerAddr.toString()] -= value;
    (*balance)[owner.toString()] += value;
    DIPC_EMIT_EVENT(Tranfer, &(callerAddr.toString()[0]), &(owner.toString()[0]), value);
}

uint64_t TestToken::getBalance(const char* own){
    prints(own);
    std::string ownerStr = CharToAddress2Str(own);
    uint64_t ba =  balance.get()[ownerStr];
    printui(ba);
    DIPC_EMIT_EVENT(Tranfer, "", own, ba);
    return ba;
}

uint64_t TestToken::getApproveBalance(const char* from, const char* approved){
    prints("getApproveBalance");
    prints(from);
    prints(approved);
    std::string fromStr = CharToAddress2Str(from);
    std::string approvedStr = CharToAddress2Str(approved);
    uint64_t re = balance.get()[fromStr+approvedStr];
    printui(re);
    DIPC_EMIT_EVENT(Tranfer, from, approved, re);
    return re;
}
