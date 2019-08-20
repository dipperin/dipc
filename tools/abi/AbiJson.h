#pragma once
#include "boost/filesystem.hpp"

#include "AbiDef.h"

namespace dipc {
    void outputJsonABI(const ABIDef &abiDef, const ContractDef &contractDef, std::ofstream &fs);
}

