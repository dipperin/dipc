#pragma once
#include <dipc/dipc.hpp>
using namespace dipc;

char tmp[8] = "supply2";
char bal[9] = "balance2";
char na[6] = "dipc2";
char ow[5] = "own2";
char sy[8] = "symbol2";
class call : public Contract {
public: 
    EXPORT void init(char* tokenName, char* sym, uint64_t supply);
    PAYABLE void callInit(const char* contractAddr, const char* tokenName, const char* sym, uint64_t supply);
    PAYABLE void callTokenTransfer(const char* contractAddr, const char* to, uint64_t value);
    PAYABLE void delegateCallTokenTransfer(const char* contractAddr, const char* to, uint64_t value);
    PAYABLE bool transfer(const char* to, uint64_t value);
    CONSTANT uint64_t callGetBalance(const char* contractAddr, const char* own);
    CONSTANT uint64_t delegateCallGetBalance(const char* contractAddr, const char* own);
    CONSTANT uint64_t getBalance(const char* own);
private: 
    String<na> name ;
    String<sy> symbol;
    Map<bal, std::string, uint64_t >  balance;
    Uint64<tmp> total_supply;
    String<ow> owner;
    inline void isZeroAddress(std::string const& _s){
        DipcAssert(_s != "0x0");
    }    
};

DIPC_EVENT(CallTransfer, const char*, const char*, uint64_t);
DIPC_EVENT(CallGetBalance, const char*, uint64_t);