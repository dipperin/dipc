#pragma once
#include <dipc/dipc.hpp>
using namespace dipc;


char tokenName[] = "token";
class newtoken : public Contract {
 public: 
 EXPORT void init();

 CONSTANT void hello(const std::string name);
 PAYABLE void can_payable();
 private:
 String<tokenName> tn;
};
DIPC_EVENT(payable, std::string, uint64_t);