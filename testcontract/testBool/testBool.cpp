#include "testBool.hpp"

EXPORT void testBool::init(bool insert) {
	*_insert = insert;
	//std::string test = "test";
	DIPC_EMIT_EVENT(booltest, insert, "test");
}

EXPORT bool testBool::test() {
	return true;
};

