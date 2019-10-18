#include "demo.hpp"


// "0x0000D36F282D8925B16Ed24CB637475e6a03B01E1056"
EXPORT void demo::init(const char* to) {
   //std::string toStr = CharToAddress2Str(to);
   Address addr = Address(to, 22);
   std::string addrStr = addr.toString();
   print(addrStr);
   //DIPC_EMIT_EVENT(Trans, "addrStr", addrStr);
}

CONSTANT void demo::hello(const std::string name) {
  println("hello", name);
}

PAYABLE void demo::can_payable() {
  println("you can use PAYABLE identifier to transfer CSK");
}

