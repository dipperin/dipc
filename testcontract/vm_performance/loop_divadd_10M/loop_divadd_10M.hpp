#pragma once

#include <dipc/dipc.hpp>
using namespace dipc;
class loop_divadd_10M :public Contract 
{
public:
    EXPORT void init();
    EXPORT uint32_t loop_divadd_10M_test();
};
