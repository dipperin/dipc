#include <algorithm>
#include <regex>
#include "AbiGenerator.h"
#include "Exception.h"
#include "Common.h"
#include "Log.h"
using namespace std;
namespace cl = llvm::cl;
using namespace clang;

namespace dipc {

    static string convertBuildinType(const string &typeName){
        regex r(R"(char\s*\[\d+\])");
        smatch smatch;
        if (regex_search(typeName, smatch, r)) {
           return "char *";
        }
        return typeName;
    }
                    
    void ABIGenerator::getRealName(clang::QualType &type, clang::ASTContext* astContext, std::string &typeName, std::string &realTypeName) {
        if (isa<TypedefType>(type.getTypePtr())) {
            const auto* tdDecl = type->getAs<clang::TypedefType>()->getDecl();
            auto underlyingType = tdDecl->getUnderlyingType().getUnqualifiedType();
            LOGDEBUG << "xxx:" << tdDecl->getName().str();
            if (typeName.empty()) {
                LOGDEBUG << "set typeName" << typeName;
                typeName = tdDecl->getName().str();
            }
            if (isa<ArrayType>(underlyingType.getTypePtr())){
                LOGDEBUG << "typeName:" << typeName << " is array";
            }

            if (isa<TypedefType>(underlyingType.getTypePtr())){
                LOGDEBUG << "get typename again type:" << typeName << " realTypeName:" << realTypeName;
                getRealName(underlyingType, astContext, typeName, realTypeName);
            } else {
                auto qtType = clang::TypeName::getFullyQualifiedType(underlyingType, *astContext);
                realTypeName = convertBuildinType(qtType.getAsString());
            }

        } else {
            auto qtType = clang::TypeName::getFullyQualifiedType(type, *astContext);
            realTypeName = convertBuildinType(qtType.getAsString());
        }
        LOGDEBUG << "typeName:" << typeName << "  realTypeName:" << realTypeName;
    }

    // void ABIGenerator::handleTagDeclDefinition(TagDecl* tagDecl) {
    //     clang::ASTContext* astContext = &tagDecl->getASTContext();
    //     const auto& sm = compilerInstance->getSourceManager();
    //     if (tagDecl->getName().str() == contract) {
    //         if (contract == "Token"){
    //             LOGDEBUG << "contract == Token";
    //         } else {
    //             LOGDEBUG << "contract != Token";
    //         }
    //         LOGDEBUG << "decl name:[" << tagDecl->getName().str() << "] contract:[" <<contract << "]";
    //         const auto* recDecl =  dyn_cast<CXXRecordDecl>(tagDecl);
    //         if (recDecl == nullptr) return;
    //         for (const CXXMethodDecl* method : recDecl->methods()) {
    //             SourceRange sr = method->getSourceRange();
    //             SourceLocation b(sr.getBegin()), e(sr.getEnd());
    //             auto funcEntire = string(sm.getCharacterData(b), sm.getCharacterData(e));
    //             string funcEnd = string(sm.getCharacterData(e));
    //             LOGDEBUG << "funcEntire : " << funcEntire  << "; funcEnd : " << funcEnd;
    //             string methodName = method->getNameAsString();
    //             if (std::find_if(actions.begin(), actions.end(), [methodName] (const string& o) -> bool {return o == methodName;}) == actions.end()) {
    //                 continue;
    //             }
    //             LOGDEBUG << "method :" << methodName;
    //             ABI abi;
    //             abi.methodName = method->getNameAsString();
    //             if (method->isConst() || method->isDipcConstant()){
    //                 abi.isConst = true;
    //             } else if (method->isDipcPayable()){
    //                 abi.isPayable = true;
    //             } 
    //             clang::QualType returnType = method->getReturnType();
    //             getRealName(returnType, astContext, abi.returnType.typeName, abi.returnType.realTypeName);

    //             for (const auto *p : method->parameters()) {
    //                 clang::QualType qt = p->getOriginalType().getNonReferenceType();
    //                 abi.args.push_back(p->getNameAsString());
    //                 LOGDEBUG << "parame name:" << p->getNameAsString();
    //                 string typeName;
    //                 string realTypeName;
    //                 getRealName(qt, astContext, typeName, realTypeName);
    //                 if (!isBuildinType(realTypeName)) {
    //                     throw Exception() << ErrStr(methodName + ":" + p->getNameAsString() + ":" + realTypeName + " is not buildin type");
    //                 }
    //                 abi.types.push_back({typeName, realTypeName});
    //             }
    //             abiDef.abis.push_back(abi);
    //         }
    //         LOGDEBUG << "abis size:" << abiDef.abis.size();
    //     }
    // }

    void ABIGenerator::handleTagDeclDefinition(TagDecl* tagDecl) {
        clang::ASTContext* astContext = &tagDecl->getASTContext();
        const auto& sm = compilerInstance->getSourceManager();
        //sm.getFileManager().getFile().
        if (tagDecl->getName().str() == contract) {
            if (contract == "Token"){
                LOGDEBUG << "contract == Token";
            } else {
                LOGDEBUG << "contract != Token";
            }
            LOGDEBUG << "decl name:[" << tagDecl->getName().str() << "] contract:[" <<contract << "]";
            const auto* recDecl =  dyn_cast<CXXRecordDecl>(tagDecl);
            if (recDecl == nullptr) return;
            for (const CXXMethodDecl* method : recDecl->methods()) {
                SourceRange sr = method->getSourceRange();
                SourceLocation b(sr.getBegin()), e(sr.getEnd());
                auto funcEntire = string(sm.getCharacterData(b), sm.getCharacterData(e));
                string funcEnd = string(sm.getCharacterData(e));
                LOGDEBUG << "funcEntire : " << funcEntire  << "; funcEnd : " << funcEnd;
                string methodName = method->getNameAsString();
                if (std::find_if(actions.begin(), actions.end(), [methodName] (const string& o) -> bool {return o == methodName;}) == actions.end()) {
                    continue;
                }
                LOGDEBUG << "method :" << methodName;
                ABI abi;
                abi.methodName = method->getNameAsString();
                auto constant = method->isConst() || method->isDipcConstant();
                uint8_t macroNum = 0;
                if (constant){
                    abi.isConst = true;
                    abi.isPayable = false;
                    abi.modifier = "CONSTANT";
                    macroNum++;
                } 
                if (method->isDipcPayable()){
                    abi.isPayable = true;
                    abi.modifier = "PAYABLE";
                    macroNum++;
                } 
                if (method->isDipcExport()){
                    abi.isPayable = false;
                    abi.modifier = "EXPORT";
                    macroNum++;
                }
                LOGDEBUG << "abi.methodName : " << abi.methodName << "; abi.isConst : " << (method->isConst() || method ->isDipcConstant())   << "; abi.isPayable :" << method->isDipcPayable() << "; abi.isExport :" << method->isDipcExport() ;
                if (macroNum > 1) {
                    std::cerr << "ERROR!!! ERROR!!!  one method can't have more than one Macro!!!" ;
                    exit(1);
                }

                clang::QualType returnType = method->getReturnType();
                getRealName(returnType, astContext, abi.returnType.typeName, abi.returnType.realTypeName);

                for (const auto *p : method->parameters()) {
                    clang::QualType qt = p->getOriginalType().getNonReferenceType();
                    abi.args.push_back(p->getNameAsString());
                    LOGDEBUG << "parame name:" << p->getNameAsString();
                    string typeName;
                    string realTypeName;
                    //string originTypeName;
                    getRealName(qt, astContext, typeName, realTypeName);
                    if (!isBuildinType(realTypeName)) {
                        throw Exception() << ErrStr(methodName + ":" + p->getNameAsString() + ":" + realTypeName + " is not buildin type");
                    }
                    abi.types.push_back({typeName, realTypeName});
                }
                abiDef.abis.push_back(abi);
            }
            LOGDEBUG << "abis size:" << abiDef.abis.size();
        }
    }
}
