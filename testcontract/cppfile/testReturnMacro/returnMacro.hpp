#pragma once
#include <dipc/contract.hpp>
#include <dipc/print.hpp>
#include <dipc/common.h>

class testReturn : public dipc::Contract{
    public:

    testReturn():data("hello world"){}

    void init();

    int printString();

    private:
    char* data;
};

DIPC_ABI(testReturn, init);
DIPC_ABI(testReturn, printString);