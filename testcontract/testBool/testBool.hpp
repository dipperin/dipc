#pragma once
#include <dipc/dipc.hpp>
using namespace dipc;

char insertc[] = "insert";
class testBool : public Contract {
 public: 
  EXPORT void init(bool insert);
  EXPORT bool test();
private:
  Bool<insertc> _insert;
};

DIPC_EVENT(booltest, bool, char *);

