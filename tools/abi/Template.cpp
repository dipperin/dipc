//
// Created by zhou.yang on 2018/10/9.
//

#include "Template.h"

using namespace std;

namespace dipc {

    string generateAbiCPlusPlus(ContractDef &contractDef, ABIDef &abiDef) {
        string code = "//dipc autogen begin\n";
        code += "extern \"C\" { \n";
        for (auto method : abiDef.abis) {
            std::string returnType = method.returnType.realTypeName == "_Bool" ? "bool" : method.returnType.realTypeName;
            code += returnType + " ";
            code += method.methodName + "(";
            for (int i = 0; i < method.args.size(); ++i) {
                code += (method.types[i].realTypeName == "_Bool" ? "bool" : method.types[i].realTypeName)+ " ";
                code += method.args[i];
                if (i != method.args.size()-1) {
                    code += ",";
                }
            }
            code += ") {\n";
            code += contractDef.fullName + " ";
            string var = contractDef.name + "_dipc";
            code += var + ";\n";
            if (method.returnType.realTypeName != "void"){
                code += "return ";
            }
            code += var + "." + method.methodName + "(";
            for (int i = 0; i < method.args.size(); ++i) {
                code += method.args[i];
                if (i != method.args.size()-1) {
                    code += ",";
                }
            }
            code += ");\n}\n";
        }
        code += "\n}\n//dipc autogen end";
        return code;
    }
}
