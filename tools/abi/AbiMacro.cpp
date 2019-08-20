#pragma once
#include <iostream>
#include <regex>

#include "llvm/ADT/STLExtras.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Frontend/FrontendActions.h"
#include "StringUtil.h"
#include "AbiMacro.h"
#include "Log.h"
#include "Common.h"
#include "Exception.h"

//  Ashbur  for test begin 
// Ashbur for test  end
using namespace std;
namespace cl = llvm::cl;
using namespace clang;



namespace dipc{

    void FindMacroAction::ExecuteAction() {
        // getCompilerInstance().getPreprocessor().SetMacroExpansionOnlyInDirectives();
        getCompilerInstance().getPreprocessor().addPPCallbacks(
                llvm::make_unique<MacroCallback>(getCompilerInstance(), *this)
                );
        PreprocessOnlyAction::ExecuteAction();
    }

    void MacroCallback::handleEvent(const Token &token, const MacroDefinition &md, SourceRange range, const MacroArgs *args) {
        const auto& sm = compilerInstance.getSourceManager();
        auto file_name = sm.getFilename(range.getBegin());
        clang::SourceLocation b(range.getBegin()), _e(range.getEnd());
        clang::SourceLocation e(clang::Lexer::getLocForEndOfToken(_e, 0, sm, compilerInstance.getLangOpts()));
        auto macrostr = string(sm.getCharacterData(b), sm.getCharacterData(e)-sm.getCharacterData(b));

        regex r(R"(DIPC_EVENT\s*\(\s*(.+?)\s*,((?:.+?)*)\s*\))");
        smatch smatch;
        auto res = regex_search(macrostr, smatch, r);
        Event event;
        string name = smatch[1];
        trim(name);
        event.name = name;
        vector<string> eventArgs;

        split(smatch[2], eventArgs,",");
        for (size_t i = 0; i < eventArgs.size(); i++) {
            trim(eventArgs[i]);
            if (!isBuildinType(eventArgs[i])) {
                throw Exception() << ErrStr(name +  + ":" + eventArgs[i] + " is not buildin type");
            }
            event.args.push_back(eventArgs[i]);
        }
        act.contractDef.eventDef.events.push_back(event);
    }

    void MacroCallback::handleMacro(const Token &token, const MacroDefinition &md, SourceRange range, const MacroArgs *args, string nameMacro) {
        const auto& sm = compilerInstance.getSourceManager();
        auto file_name = sm.getFilename(range.getBegin());
        clang::SourceLocation b(range.getBegin()), _e(range.getEnd());
        clang::SourceLocation e(clang::Lexer::getLocForEndOfToken(_e, 0, sm, compilerInstance.getLangOpts()));
        auto longstr = string(sm.getCharacterData(b));
        auto str = string(sm.getCharacterData(b), sm.getCharacterData(e));
        auto macrostr = string(sm.getCharacterData(b), sm.getCharacterData(e)-sm.getCharacterData(b));
        LOGDEBUG << "file_name : " << file_name.str() << endl;
        LOGDEBUG << "Export macrostr:" << macrostr << "; long str :" << str << "; longstr :" << longstr;
        //regex r(R"(DIPC_EVENT\s*\(\s*(.+?)\s*,((?:.+?)*)\s*\))");
        //regex r(R"(EXPORT\s+(.+?)\s+(.*\())");
        string regexStr = nameMacro + R"(\s+(.+?)\s+(.*\())";
        regex r(regexStr);
        //regex r(R"(EXPORT\s+(.+?)\s+(.*\())");
        longstr = findAndGetBeforeFirst(longstr, ")");
        smatch smatch;
        auto res = regex_search(longstr, smatch, r);

        //act.contractDef.fullName = smatch[1].str();
        //act.contractDef.name = removeNamespace(smatch[1].str());
        //string action = smatch[2].str();
        string action = getName(smatch[2].str());
        string name = findAndGetBeforeFirst(action, "::");
        if(name != ""){
            act.contractDef.name = name;
            act.contractDef.fullName = name;
        } else {
            return;
        }
        action = findAndGetAfterFirst(action, "::");
        //trim(action);
        act.actions.push_back(action);
        LOGDEBUG << "Export macrostr:" << macrostr
            << "  contract:" << smatch[1].str()
            << "  actions_str:" << action 
            << endl;
    }

    void MacroCallback::MacroExpands (const Token &token, const MacroDefinition &md, SourceRange range, const MacroArgs *args) {
        auto *id = token.getIdentifierInfo();
        //LOGDEBUG << "MacroExpands token name start:" << id->getName().str();
        if (id == nullptr) return;
        //LOGDEBUG << "MacroExpands token name:" << id->getName().str();
        if (id->getName() == "DIPC_EVENT"){
            LOGDEBUG << "get EVENT:" <<  id->getName().str();
            handleEvent(token, md, range, args);
            return;
        }

        if (id->getName() == "EXPORT" || id->getName() == "PAYABLE" || id->getName() == "WITHDRAW" || id->getName() == "CONSTANT"){
            LOGDEBUG << "get macro:" << id->getName().str();
            handleMacro(token, md, range, args, id->getName().str());
            return;
        }

    }

     std::string removeNamespace(std::string fullName) {
        int i = fullName.size();
        int on_spec = 0;
        int colons = 0;
        while(--i >= 0) {
            if(fullName[i] == '>') {
                ++on_spec; colons=0;
            } else if(fullName[i] == '<') {
                --on_spec; colons=0;
            } else if(fullName[i] == ':' && !on_spec) {
                if (++colons == 2)
                    return fullName.substr(i+2);
            } else {
                colons = 0;
            }
        }
        return fullName;
    }

    std::string getName(std::string name){
        int i = name.size();
        while(--i >= 0){
            if(name[i] == '('){
                name = name.substr(0,i);
                trim(name);
                return name;
            }
        }
    
      return "";
    }

    std::string findAndGetBeforeFirst(std::string name, std::string findStr){
        int position = name.find(findStr);
        if(position != name.npos){
            name = name.substr(0,position);
            trim(name);
            return name;
        } 
        return "";
    }

    std::string findAndGetAfterFirst(std::string name, std::string findStr){
        int position = name.find(findStr);
        if(position != name.npos){
            name = name.substr(position + findStr.size());
            trim(name);
            return name;
        } 
        return "";
    }
}
