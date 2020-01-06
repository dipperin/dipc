#pragma once
#include <dipc/dipc.hpp>
using namespace dipc;

char st[] = "startB";
char num[] = "calculatedFuNumberB";
class fibonacciBalance : public Contract {
 public: 
    EXPORT void init(char * test);
    EXPORT void setStart(uint64_t _start);
    EXPORT void callSetFibonacci(const char* contractAddr, uint64_t count);
    EXPORT void delegateCallSetFibonacci(const char* contractAddr, uint64_t count);
    CONSTANT uint64_t getCalculatedFiNumber();
    Uint64<st> start;
    Uint64<num> calculatedFiNumber;
};

