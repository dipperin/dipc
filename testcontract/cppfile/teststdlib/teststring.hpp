#pragma once
#include <string>
#include <dipc/contract.hpp>
#include <dipc/print.hpp>

class testString : public dipc::Contract{
    public:

    testString():data("hello world"){}

    void init();

    void printString();

    private:
    std::string data;
};

DIPC_ABI(testString, init);
DIPC_ABI(testString, printString);