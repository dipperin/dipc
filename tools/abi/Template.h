#pragma once

#include "AbiDef.h"
#include <string>
namespace dipc {

    std::string generateAbiCPlusPlus(ContractDef &contractDef, ABIDef &abiDef);
}