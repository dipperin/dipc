#pragma once
#include <string>
#include <vector>

namespace dipc {
    struct TypdeDef {
        TypdeDef() = default;
        TypdeDef(std::string typeName, std::string realTypeName)
            :typeName(typeName), realTypeName(realTypeName){}
        TypdeDef(std::string typeName, std::string realTypeName, std::string originName)
            :typeName(typeName), realTypeName(realTypeName), originName(originName){}

        std::string typeName;
        std::string realTypeName;
        std::string originName;
    };

    struct ABI {
        std::string methodName;
        std::vector<std::string> args;
        std::vector<TypdeDef> types;
        std::string modifier;
        TypdeDef returnType;
        bool isConst = false;
        bool isPayable = false;
    };

    struct ABIDef {
        std::vector<ABI> abis;
    };

    struct Event {
        std::string name;
        std::vector<std::string> args;
    };
    struct EventDef {
        std::vector<Event> events;
    };

    struct ContractDef{
        std::string fullName;
        std::string name;
        EventDef eventDef;
    };
}
