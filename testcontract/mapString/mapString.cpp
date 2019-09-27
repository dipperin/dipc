#include "mapString.hpp"

void mapString::init() {}

void mapString::setBalance(char* key, int value) {
	std::string strKey;
	strKey = key;
	(*balance)[strKey] = value;
};

CONSTANT int mapString::getBalance(char* key) {
	return balance.get()[key];
};
