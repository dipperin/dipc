#pragma once
#include <dipc/dipc.hpp>
using namespace dipc;

char temp[8] = "balance";
class mapString : public Contract {
 public: 
  void init();
  Map<temp, std::string, int> balance;
  void setBalance(char* key, int value);
  CONSTANT int getBalance(char* key);
};

// You must define ABI here.
DIPC_ABI(mapString, init);
DIPC_ABI(mapString, setBalance);
DIPC_ABI(mapString, getBalance);

