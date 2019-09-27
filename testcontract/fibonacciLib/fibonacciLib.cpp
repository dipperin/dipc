#include "fibonacciLib.hpp"

EXPORT void fibonacciLib::init() {}

EXPORT void fibonacciLib::setStart(uint64_t _start) {
  *start = _start;
}

EXPORT void fibonacciLib::setFibonacci(uint64_t n) {
  *calculatedFiNumber = fibonacci(n);
  DIPC_EMIT_EVENT(Number, *calculatedFiNumber);
}

CONSTANT uint64_t fibonacciLib::fibonacci(uint64_t n) {
  if (n == 0) {
    return *start;
  }
  else if (n == 1) {
    return *start + 1;
  }
  else {
    return fibonacci(n-1) + fibonacci(n-2);
  }
}

CONSTANT uint64_t fibonacciLib::getCalculatedFiNumber() {
  return *calculatedFiNumber;
}