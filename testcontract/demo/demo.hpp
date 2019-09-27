#pragma once
#include <dipc/dipc.hpp>
using namespace dipc;

class demo : public Contract {
 public: 
 EXPORT void init();

 CONSTANT void hello(const std::string name);
 PAYABLE void can_payable();
};

