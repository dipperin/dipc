#include "newtoken.hpp"

EXPORT void newtoken::init() {
  *tn = "newtoken";
}

CONSTANT void newtoken::hello(const std::string name) {
  println("hello", name);
}

PAYABLE void newtoken::can_payable() {
  println("you can use PAYABLE identifier to transfer CSK");
  DIPC_EMIT_EVENT(payable, "this is payable function", 64);
}

