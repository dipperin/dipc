#include "mapString.hpp"

EXPORT void mapString::init() {}

EXPORT void mapString::setBalance(char* key, int value) {
	std::string strKey;
	strKey = key;
	(*balance)[strKey] = value;
};

CONSTANT int mapString::getBalance(char* key) {
	return balance.get()[key];
};
