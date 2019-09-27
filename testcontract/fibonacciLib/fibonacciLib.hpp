#pragma once
#include <dipc/dipc.hpp>
using namespace dipc;

char st[] = "startA";
char num[] = "calculatedFiNumberA";
class fibonacciLib : public Contract {
 public: 
    EXPORT void init();
    EXPORT void setStart(uint64_t _start);
    EXPORT void setFibonacci(uint64_t n);
    CONSTANT uint64_t fibonacci(uint64_t n);
    CONSTANT uint64_t getCalculatedFiNumber();
 private: 
    Uint64<st> start;
    Uint64<num> calculatedFiNumber;
};
DIPC_EVENT(Number, uint64_t);
