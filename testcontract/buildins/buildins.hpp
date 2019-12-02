#pragma once
#include <dipc/dipc.hpp>
using namespace dipc;

class buildins : public Contract
{
public:
  EXPORT void init();
  EXPORT void arithmeticTest();
};