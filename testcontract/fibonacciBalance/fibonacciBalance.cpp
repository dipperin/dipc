#include "fibonacciBalance.hpp"

EXPORT void fibonacciBalance::init() {}

EXPORT void fibonacciBalance::setStart(uint64_t _start) {
  *start = _start;
}

EXPORT void fibonacciBalance::callSetFibonacci(const char* contractAddr, uint64_t count) {
  DeployedContract contract(contractAddr);
  contract.call("setFibonacci", count);
}

EXPORT void fibonacciBalance::delegateCallSetFibonacci(const char* contractAddr, uint64_t count) {
  DeployedContract contract(contractAddr);
  contract.delegateCall("setFibonacci", count);
}

CONSTANT uint64_t fibonacciBalance::getCalculatedFiNumber() {
  return *calculatedFiNumber;
}