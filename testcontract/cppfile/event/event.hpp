#pragma once
#include <dipc/dipc.hpp>
using namespace dipc;

class envEvent : public Contract {
 public: 
  void init(char *tokenName, char *sym, uint64_t supply);

  void hello(const char* name, int64_t num);
};

// You must define ABI here.
DIPC_ABI(envEvent, init);
DIPC_ABI(envEvent, hello);
DIPC_EVENT(logName, const char*, int64_t);