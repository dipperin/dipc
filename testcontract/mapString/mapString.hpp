#pragma once
#include <dipc/dipc.hpp>
using namespace dipc;

char temp[8] = "balance";
class mapString : public Contract {
 public: 
  EXPORT void init();
  Map<temp, std::string, int> balance;
  EXPORT void setBalance(char* key, int value);
  CONSTANT int getBalance(char* key);
};
