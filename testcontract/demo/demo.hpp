#pragma once
#include <dipc/dipc.hpp>
using namespace dipc;

class demo : public Contract {
 public: 
 EXPORT void init(const char* to);

 CONSTANT void hello(const std::string name);
 PAYABLE void can_payable();
};
//DIPC_EVENT(Trans, const char*, const char*, );

