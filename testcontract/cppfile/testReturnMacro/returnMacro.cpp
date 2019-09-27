#include "returnMacro.hpp"

void testReturn::init(){}

int testReturn::printString(){
    dipc::print(data);
    RETURN(1)
}


