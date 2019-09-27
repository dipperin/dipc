#include "event.hpp"

void envEvent::init(char *tokenName, char *sym, uint64_t supply) {
  print("event init test\r\n");
}

void envEvent::hello(const char* name, int64_t num) {
  // println("hello", name);
  DIPC_EMIT_EVENT(logName, name, num);
}