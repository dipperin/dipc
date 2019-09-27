#include "demo.hpp"

EXPORT void demo::init() {}

CONSTANT void demo::hello(const std::string name) {
  println("hello", name);
}

PAYABLE void demo::can_payable() {
  println("you can use PAYABLE identifier to transfer CSK");
}

