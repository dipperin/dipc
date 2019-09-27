#pragma once
#include <dipc/dipc.hpp>
using namespace dipc;

class testBool : public Contract {
 public: 
  EXPORT void init();
  EXPORT bool test();
};

