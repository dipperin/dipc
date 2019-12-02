#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <iostream>
#include <AbiDef.h>

using namespace clang;
using namespace clang::ast_matchers;
using namespace dipc;

bool isImplContract = false;

class ClassImplCheck : public MatchFinder::MatchCallback {
public:
    ClassImplCheck(ContractDef &contractDef): contractDef(contractDef){}

    // virtual void run(const MatchFinder::MatchResult& result){
    //     //result.Nodes

    //     //try{
    //         std::cout << "step in classImplCheck"  << std::endl;

    //         CXXRecordDecl const * recordDeclGet =  result.Nodes.getNodeAs<CXXRecordDecl>("contract");

    //         std::cout << "step in classImplCheck 2 "  << std::endl;

    //         CXXRecordDecl const * parentDeclGet =  result.Nodes.getNodeAs<CXXRecordDecl>("parent");

    //         std::cout << "step in classImplCheck 3 "  << std::endl;
    //         //if(recordDeclGet != nullptr && recordDeclLocal == nullptr) {
    //         if(recordDeclGet != nullptr) {
            
    //             recordDeclLocal = recordDeclGet;
    //             std::cout << "recordDeclLocal addr "  << recordDeclLocal << std::endl;  
    //             recordName = recordDeclLocal->getNameAsString();
    //             recordDeclLocal->
    //             std::cout << "recordDeclLocal isn't nullptr" << std::endl;
    //         } 
    //         if (parentDeclGet != nullptr && parentDeclLocal == nullptr){
    //         //if (parentDeclGet != nullptr){
    //             parentDeclLocal = parentDeclGet;
    //             std::cout << "parentDeclLocal addr "  << parentDeclLocal << std::endl;  

    //             parentName = parentDeclLocal->getNameAsString();
    //             std::cout << "parentDeclLocal isn't nullptr" << std::endl;
    //         }

    //         if ((recordName != "" && parentDeclLocal != nullptr) || (parentName != "" && recordDeclLocal != nullptr)){
    //                 std::cout << "ClassImplCheck  recordName " << recordName << "  parentDeclLocal "  <<   parentName <<std::endl;

    //                 //recordDeclLocal
    //                 // if (recordDeclLocal->isDerivedFrom(parentDeclLocal)){
    //                 //     std::cout << "recordDeclLocal is derive from parentDeclLocal " << std::endl;
    //                 // }
    //                 //recordDeclLocal->isDerivedFrom(hasName("Contract"));
    //                 // if(recordName == "" || recordName != contractDef.fullName) {
    //                 //     std::cerr << "ERROR!!! ERROR!!! contract class must implement Contract"  <<  std::endl; 
    //                 //     exit(1);
    //                 // }

    //                 if(recordDeclLocal != nullptr && parentDeclLocal != nullptr){
    //                     if (recordDeclLocal->isDerivedFrom(parentDeclLocal)){
    //                         std::cout << "recordDeclLocal is derive from parentDeclLocal " << std::endl;
    //                     }
    //                     if (parentName != "Contract"){
    //                         std::cerr << "the" << std::endl;
    //                     }
    //                     std::cout << "both are not nullptr" << std::endl;
    //                 }

    //                 if (recordDeclLocal != nullptr && parentDeclLocal == nullptr){
    //                     std::cerr << "can't find the parent " << std::endl;
    //                     exit(1);
    //                 }

    //                 std::cout << "ClassImplCheck  recordName " << recordName << std::endl;
    //         }
        
    // }


    virtual void run(const MatchFinder::MatchResult& result){
            //std::cout << "step in classImplCheck"  << std::endl;

            CXXRecordDecl const * recordDeclGet =  result.Nodes.getNodeAs<CXXRecordDecl>("contract");
           
           
            if(recordDeclGet != nullptr) {
            
                isImplContract = true;
            }
        
    }
    
private:    
    ContractDef &contractDef;

};