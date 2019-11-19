#include <fstream>
#include <iostream>
#include <streambuf>
#include <regex>
#include <string>
#include <vector>
#define RAPIDJSON_HAS_STDSTRING 1

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/Driver/Options.h"
#include "clang/Lex/Preprocessor.h"

#include "boost/filesystem.hpp"
#include "boost/random.hpp"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/QualTypeNames.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Lex/MacroArgs.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Sema/Sema.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"

#include "AbiDef.h"
#include "AbiGenerator.h"
#include "AbiJson.h"
#include "AbiMacro.h"
#include "Exception.h"
#include "Log.h"
#include "StringUtil.h"
#include "Template.h"

using namespace std;
namespace cl = llvm::cl;
namespace fs = boost::filesystem;
namespace br = boost::random;

using namespace clang;
using namespace dipc;

static cl::OptionCategory abiGeneratorOptions("ABI generator options");

static cl::opt<std::string> logPath(
    "log_path", cl::desc("log path, default = /tmp/[randomdir]/dipc-[tool name].log"),
    cl::cat(abiGeneratorOptions));

static cl::opt<std::string> logLevel("log_level",
                                     cl::desc("log level, default = debug"),
                                     cl::cat(abiGeneratorOptions),
                                     cl::init("debug"));

// static cl::opt<bool> verbose("verbose", cl::desc("log on-off, default = true"),
//                              cl::cat(abiGeneratorOptions), cl::init(true));

static cl::opt<bool> verbose("verbose", cl::desc("log on-off, default = true"),
cl::cat(abiGeneratorOptions));

static cl::opt<std::string> abigen_output_opt(
    "abigen_output", cl::desc("ABIGEN output, default = ./<filename>.abi.json"),
    cl::cat(abiGeneratorOptions));

static cl::opt<std::string> exports_output_opt("exports_output",
                                               cl::desc("exports output"),
                                               cl::cat(abiGeneratorOptions));

static cl::opt<std::string> abidef_output_opt("abidef_output",
                                              cl::desc("abi define output"),
                                              cl::cat(abiGeneratorOptions));

std::unique_ptr<tooling::FrontendActionFactory> CreateFindMacroFactory(
    ContractDef &contractDef, vector<string> &actions)
{
  struct FrontendMacroActionFactory : public tooling::FrontendActionFactory
  {
    ContractDef &contractDef;
    vector<string> &actions;

    FrontendMacroActionFactory(ContractDef &contractDef,
                               vector<string> &actions)
        : contractDef(contractDef), actions(actions) {}

    clang::FrontendAction *create() override
    {
      return new FindMacroAction(contractDef, actions);
    }
  };

  LOGDEBUG << "start create_find_macro_factory";
  return std::unique_ptr<tooling::FrontendActionFactory>(
      new FrontendMacroActionFactory(contractDef, actions));
}

std::unique_ptr<tooling::FrontendActionFactory> createFactory(
    const string &contract, const vector<string> &actions, ABIDef &abiDef)
{
  struct ABIFrontendActionFactory : public tooling::FrontendActionFactory
  {
    const string &contract;
    const vector<string> &actions;
    ABIDef &abiDef;

    ABIFrontendActionFactory(const string &contract,
                             const vector<string> &actions, ABIDef &abiDef)
        : contract(contract), actions(actions), abiDef(abiDef) {}

    clang::FrontendAction *create() override
    {
      return new ABIAction(contract, actions, abiDef);
    }
  };

  return std::unique_ptr<tooling::FrontendActionFactory>(
      new ABIFrontendActionFactory(contract, actions, abiDef));
}

enum State
{
  CODE,                  //  normal code
  SLASH,                 // "/"
  NOTE_MULTILINE,        // multiline comments
  NOTE_MULTILINE_STAR,   // multiline comments has other char '*'
  NOTE_SINGLELINE,       // singleline comments
  BACKSLASH,             // comments with char '\'
  CODE_CHAR,             // char
  CHAR_ESCAPE_SEQUENCE,  // escape char
  CODE_STRING,           // string
  STRING_ESCAPE_SEQUENCE // escape char in string
};

struct FileStatistics
{
  FileStatistics()
  {
    memset(this, 0, sizeof(*this));
  }
  int totals; // total lines
  int codes;
  int comments;
  int blanks;
};

#define CRLF (const char *)"\n"

std::string RemoveCommentAndStatistics(const std::string &code_text, FileStatistics &fs,
                                       bool set_string_to_empty = false, bool keep_comment_line = false)
{
  std::string builder = "";

  FileStatistics &one = fs;
  bool maybeblankline;
  bool maybecodeline;

  State state = CODE;
  for (int i = 0; i < code_text.length(); i++)
  {
    char c = code_text[i];

    if (i == 0)
    {
      one.totals++;
      maybeblankline = true;
      maybecodeline = false;
    }
    else if (c == '\n')
    {
      one.totals++;
      if (maybeblankline)
      {
        one.blanks++;
      }
      else if (maybecodeline)
      {
        one.codes++;
      }
      maybeblankline = true;
      maybecodeline = false;
    }
    else
    {
      if (c != '\r' && c != '\t' && c != ' ')
      {
        maybeblankline = false;
      }
    }

    switch (state)
    {
    case CODE:
      if (c == '/')
      {
        state = SLASH;
      }
      else
      {
        if (c != '\n')
          maybecodeline = true;
        builder.push_back(c);
        if (c == '\'')
        {
          state = CODE_CHAR;
        }
        else if (c == '\"')
        {
          state = CODE_STRING;
        }
      }
      break;
    case SLASH:
      if (c == '*')
      {
        state = NOTE_MULTILINE;
      }
      else if (c == '/')
      {
        state = NOTE_SINGLELINE;
      }
      else
      {
        builder.push_back('/');
        builder.push_back(c);
        state = CODE;
      }
      break;
    case NOTE_MULTILINE:
      if (c == '*')
      {
        state = NOTE_MULTILINE_STAR;
      }
      else
      {
        if (c == '\n')
        {
          if (keep_comment_line)
            builder.append(CRLF);
          one.comments++;
        }
        state = NOTE_MULTILINE;
      }
      break;
    case NOTE_MULTILINE_STAR:
      if (c == '/')
      {
        state = CODE;
        one.comments++;
      }
      else if (c == '*')
      {
        state = NOTE_MULTILINE_STAR;
      }
      else
      {
        state = NOTE_MULTILINE;
      }
      break;
    case NOTE_SINGLELINE:
      if (c == '\\')
      {
        state = BACKSLASH;
      }
      else if (c == '\n')
      {
        builder.append(CRLF);
        state = CODE;
        one.comments++;
      }
      else
      {
        state = NOTE_SINGLELINE;
      }
      break;
    case BACKSLASH:
      if (c == '\\' || c == '\r' || c == '\n')
      {
        if (c == '\n')
        {
          if (keep_comment_line)
            builder.append(CRLF);
        }
        state = BACKSLASH;
      }
      else
      {
        state = NOTE_SINGLELINE;
      }
      break;
    case CODE_CHAR:
      builder.push_back(c);
      if (c == '\\')
      {
        state = CHAR_ESCAPE_SEQUENCE;
      }
      else if (c == '\'')
      {
        state = CODE;
      }
      else
      {
        state = CODE_CHAR;
      }
      break;
    case CHAR_ESCAPE_SEQUENCE:
      builder.push_back(c);
      state = CODE_CHAR;
      break;
    case CODE_STRING:
      builder.push_back(c);
      if (c == '\\')
      {
        state = STRING_ESCAPE_SEQUENCE;
        if (set_string_to_empty)
          builder.pop_back();
      }
      else if (c == '\"')
      {
        state = CODE;
      }
      else
      {
        state = CODE_STRING;
        if (set_string_to_empty)
          builder.pop_back();
      }
      break;
    case STRING_ESCAPE_SEQUENCE:
      if (!set_string_to_empty)
        builder.push_back(c);
      state = CODE_STRING;
      break;
    default:
      break;
    }
  }

  return builder;
}

std::string GetAndInsertBeCalled(const std::string &code_text, const ABIDef &abidef, vector<std::string>& calledFuncDetail, const string &contractName)
{

  std::string finalContract;
  try{ 

    ABIDef abidefTemp = abidef;
    std::string codetext = code_text;

    regex hadImpl(contractName + "::"  + abidef.abis[0].methodName);
    smatch  hadImplSma;
    auto hadImplResult =   regex_search(code_text, hadImplSma, hadImpl);

    vector<std::string> payableFunc;
    vector<std::string> calledFunc;

    //regex rSearch(R"(PAYABLE\s*(.+?)\s*(.+?)\s*(\(.*\)\s*\{)\s*)");
    regex rSearch("");
    sort(abidefTemp.abis.begin(), abidefTemp.abis.end(), [&](const ABI &a1, const ABI &a2) -> bool { return a1.isPayable ? true : false; });
    // for(auto abis : abidefTemp.abis) {
    //   LOGDEBUG << "GetAndInsertBeCalled abis " << "  methodName  " << abis.methodName << "  isPayable " << abis.isPayable  << "  modifier " <<  abis.modifier << std::endl;
    // }
    

    for (auto abis : abidefTemp.abis)
    {
      LOGDEBUG << "abis.ispayable :" << abis.isPayable << std::endl;
      size_t pos1 = 0;
      size_t pos2 = std::string::npos;
      auto isPayable = abis.isPayable;
      if (!isPayable)
      {
        regex rExport(R"(\s*(.+?))" + abis.methodName + R"(\s*(\(.*\)\s*\{)\s*)");
        rSearch = rExport;
      }else {
        regex rPay(R"(PAYABLE\s*(.+?)\s*)" + abis.methodName +  R"(\s*(\(.*\)\s*\{)\s*)");
        rSearch = rPay;
      }

      // use the contents of the contract file that was replaced from the second loop
      if (finalContract != "")
      {
        codetext = finalContract;
      }
      //LOGDEBUG << "finalContract  content =======   : " << finalContract <<  " codetext  "  << codetext << std::endl;

      while (true)
      {
        smatch smt;
        LOGDEBUG << "finalContract  this height  " << finalContract  << " codetext height " << codetext << endl;          

        std::string searchContext = codetext.substr(pos1);
        LOGDEBUG << "searchContext  " << searchContext << endl;          

        // judge if you need to process the data of the contract file according to abi
        if (!regex_search(searchContext, smt, rSearch))
        {
          // "search" method not found directly returns codetext
          if (finalContract == "")
          {
            finalContract = codetext;
            LOGDEBUG << "finalContract  newn  new new " << finalContract << endl;
            
            break;
          }  // 
          else if (finalContract != codetext)
          {
            LOGDEBUG << "finalContract  this new  " << finalContract  << " codetext new " << codetext << endl;          
            finalContract += codetext.substr(pos1, codetext.size());
            codetext = finalContract;
            break;
          }
          break;
        }
        std::string search = smt[0].str();
        std::string out = isPayable ? "payable : " : " Export :";
        LOGDEBUG << "log out out "  <<  out << search << endl;
        //LOGDEBUG << "smt[1].str()" << smt[1].str() << endl;
        //LOGDEBUG << "smt[2].str()" << smt[2].str() << endl;
        std::string action = getName(smt[0].str());
        //LOGDEBUG << out << action << endl;
        std::string name = findAndGetAfterFirst(action, "::");
        LOGDEBUG << "name : " << name << "   pos1  "  << pos1 << endl;


        pos2 = codetext.find(search, pos1);
        if (pos2 == std::string::npos)
        {
          finalContract += codetext.substr(pos1, codetext.size());
          codetext = finalContract;
          //LOGDEBUG << "codetext =======   : " << codetext << std::endl;
          break;
        }
        pos1 = pos2;
        LOGDEBUG << "pos2  " << pos2  << "   pos1  "  << pos1 << endl;

        //pos1 = pos2 + payable.length();

        // find the entire method starting from the pos1 position
        int cnt = 0;
        bool flg = false;
        for (size_t i = pos1; i < codetext.length(); i++)
        {
          char c = codetext[i];

          switch (c)
          {
          case '{':
            if (cnt == 0)
              flg = true;
            cnt++;
            break;
          case '}':
            cnt--;
            break;
          default:
            break;
          }
          if (flg && cnt == 0)
          {
            pos2 = i + 1;
            break;
          }
        }

        LOGDEBUG << "pos2 after " << pos2  << "   pos1  "  << pos1 << endl;

        // get the content before the searched method
        // if(finalContract == ""){
        //     finalContract = codetext.substr(0, pos1);
        // }
        //LOGDEBUG << "codetext  before "  << codetext  << std::endl;

        finalContract = codetext.substr(0, pos1);
        LOGDEBUG << "finalContract  first "  << finalContract  << std::endl;
        LOGDEBUG << "codetext  first "  << codetext  << std::endl;
        // s  : the entire method for the current lookup
        std::string s = codetext.substr(pos1, pos2 - pos1);
        LOGDEBUG << "s  first "  << s  << std::endl;

        // If the payable method calls a non-payable method, replace the called method with the call ${funcName}_inline
        if (isPayable)
        {
          for (auto abi : abidefTemp.abis)
          {
            regex funcFind(R"(\s*)" + abi.methodName + R"(\s*\()");
            
            if (!abi.isPayable)
            {
              smatch sma;
              if (regex_search(s, sma, funcFind))
              {
                //LOGDEBUG << "sma[0].str:" << sma[0].str() << endl;
                //LOGDEBUG << "s content : " << s << endl;
      
                if (abi.methodName == "init"){
                   std::cerr << "ERROR!!! ERROR!!! can't call init method in normal method\n";
                   exit(1);
                }
                calledFunc.push_back(abi.methodName);
                sort(calledFunc.begin(), calledFunc.end());
                calledFunc.erase(unique(calledFunc.begin(), calledFunc.end()), calledFunc.end());
                // Let the payable call the export method instead of calling the generated inline method.
                s = regex_replace(s, funcFind, "\n" + abi.methodName + "_inline(");
                //LOGDEBUG << "find method name called:" << abi.methodName << endl;
                LOGDEBUG << "after repalce payable func:" << s << endl;
              }
            }
          }
        }
        else
        {
          bool notfound = true;
          // If the non-payable method is called by the payable method, insert a non-payable ${funcName}_inline method. And insert the assertion in the non-payable method the transfer amount must be 0
          for (auto cFunc : calledFunc)
          {
            if (cFunc == abis.methodName)
            {
              if (int position = s.find(search) != std::string::npos)
              {
                notfound = false;
                std::string inlineTemp;
                std::string inlineTempDetail;
                std::string funcBody = s.substr(position + search.size() - 1);
                std::string returnType = abis.returnType.realTypeName == "_Bool" ? "bool" : abis.returnType.realTypeName;

                inlineTempDetail = "\n private: inline " +returnType + " " + abis.methodName + "_inline" + smt[2].str().substr(0, smt[2].str().length() - 1);
                LOGDEBUG << "inlineTempDetail temp info : " << inlineTempDetail << std::endl;
                
                calledFuncDetail.push_back(inlineTempDetail);
                bool check =  abis.returnType.realTypeName == "_Bool";
                LOGDEBUG << "abis.returnType.realTypeName == _Bool : " <<  check  << "returnType "  << returnType <<  " abis.returnType.realTypeName  " << abis.returnType.realTypeName <<  "  abis.returnType.TypeName  " << abis.returnType.typeName << std::endl;
            
                if(hadImplResult){
                    inlineTemp = "\n  inline " + returnType + " " + contractName + "::" + abis.methodName + "_inline" + smt[2].str() + "\n" + funcBody + "\n";
                }else {
                    inlineTemp = "\n  inline " + returnType + " " + abis.methodName + "_inline" + smt[2].str() + "\n" + funcBody + "\n";
                }
                LOGDEBUG << "inlineTemp temp info : " << inlineTemp << std::endl;
              
                std::string temp = s.substr(0, position + search.size() - 1);
                temp += "u256 tempv = dipc::callValue(); \n DipcAssert( tempv == 0);\n";
                temp += s.substr(position + search.size() - 1);
                s = temp;
                s = inlineTemp + s;
                LOGDEBUG << "calledFunc temp : " << s << std::endl;
              }
            }
          }
          // Insert the assertion transfer amount  must be 0  in the non-payable method not called by the payable method
          if (notfound)
          {
            if (int position = s.find(search) != std::string::npos)
            {
              std::string temp = s.substr(0, position + search.size() - 1);
              temp += "u256 tempv = dipc::callValue(); \n DipcAssert( tempv == 0);\n";
              temp += s.substr(position + search.size() - 1);
              s = temp;
              LOGDEBUG << "calledFunc temp export: " << s << std::endl;
            }
          }
        }

        finalContract += s;
        LOGDEBUG << "search func : "  << s.c_str() ;
        //payableFunc.push_back(s);
        pos1 = pos2;
      }

      LOGDEBUG << "finalContract  final content =======   : " << finalContract  << std::endl;
      
    }
 
 }catch (Exception e) {
    std::cerr  << "Exception info "  << e.what() << std::endl;
    return "";
}

  LOGDEBUG << "finalContract  final content  last =======   : " << finalContract << std::endl;

  return finalContract;
}

void CompareHeaderAndImplFileMacro(const std::string &code_text, const ABIDef &abidef, const string& contractName, bool isImplWithHeader){
       LOGDEBUG <<  "CompareHeaderAndImplFileMacro  start "  << std::endl;
      std::string codeText = code_text;

      if (isImplWithHeader){
          int count = 0;
          bool flag = false;
          size_t pos = 0;
          for(size_t i = 0; i < codeText.size(); i++){
            char c = codeText[i];
            switch (c) {
              case '{':
                  if (count == 0){
                    flag = true;
                  }
                  count++;
                  break;
              case '}':
                  count--;
                  break;
              default:
                  break;
            }
            if(count == 0 && flag){
                pos = i + 1;
                break;
            }
          }
          codeText = code_text.substr(pos);
      }
      

      LOGDEBUG << "get codeText content "  << codeText << std::endl;

       for(auto abi : abidef.abis){
        std::string typeName = abi.returnType.typeName == "" ? abi.returnType.realTypeName : abi.returnType.typeName;
        typeName =  typeName == "_Bool" ? "bool" : typeName;
        string searchStr = R"(\s*)" + abi.modifier + R"(\s*)" + typeName + R"(\s*)"+ contractName +"::"+ abi.methodName;
        LOGDEBUG << "searchStr "  << searchStr << " code_text " << code_text << std::endl;
        LOGDEBUG << "searchStr " << searchStr << endl;
        regex searchFuncHead(searchStr);
        smatch sma;
        if (!regex_search(codeText, sma, searchFuncHead)){
           searchStr = R"(\s*)" + abi.modifier + R"(\s*)" + typeName + R"(\s*)" + abi.methodName;
           if (!regex_search(codeText, sma, searchFuncHead)){
                std::cerr <<  "ERROR: <dipc-abigen> header declare is not same with the implement file;Please make sure the function macro is same "  << std::endl;
                exit(1);
           }
        }
      }
      LOGDEBUG << "CompareHeaderAndImplFileMacro end  "  << std::endl;
}

std::string InsertFuncToHeaderFile(const std::string &code_text, vector<std::string> calledFuncDetail, const ABIDef &abidef , const std::string &filename, fs::path &randomDir, const std::string contractName, std::string outHeaderPath, bool isSaveToFile = false)
{
  std::string codetext = code_text;
  std::string tempCName = contractName;
  LOGDEBUG << "InsertFuncToHeaderFile start...." << std::endl;
  
  regex findClass(R"(class\s*)" + contractName + R"(\s*(.|\s)*?\{)");
  smatch claSma;
  // LOGDEBUG << "claSma[0].str()  :    " << claSma[0].str();
  // LOGDEBUG << "claSma[0].str()  :    " << claSma[0].str() << std::endl;
  LOGDEBUG << "InsertFuncToHeaderFile new ...." << std::endl;
  auto result = regex_search(codetext, claSma, findClass);
  LOGDEBUG << "InsertFuncToHeaderFile result ...." << result << std::endl;

  if (result)
  {
    size_t pos1 = 0;
    size_t pos2 = std::string::npos;
    std::string search = claSma[0].str();
    LOGDEBUG << "claSma[0].str()  :    " << claSma[0].str() << std::endl;
    pos2 = codetext.find(search);
    LOGDEBUG << "pos2  :    " << pos2 << std::endl;
    if (pos2 != string::npos)
    {
      pos1 = pos2;
      //pos1 = pos2 + payable.length();

      int cnt = 0;
      bool flg = false;
      for (size_t i = pos1; i < codetext.length(); i++)
      {
        char c = codetext[i];

        switch (c)
        {
        case '{':
          if (cnt == 0)
            flg = true;
          cnt++;
          break;
        case '}':
          cnt--;
          break;
        default:
          break;
        }
        if (flg && cnt == 0)
        {
          pos2 = i + 1;
          break;
        }
      }
      std::string tempText = codetext.substr(0, pos2 - 1);
      LOGDEBUG << "tempText   :  " << tempText << std::endl;

      regex hadImpl(contractName + "::"  + abidef.abis[0].methodName);
      LOGDEBUG << "filename   :  " << filename << std::endl;

      //regex nameReg(R"(.h|.hpp|.hh)");
      if(regex_search(codetext, claSma, hadImpl) || isSaveToFile){
          for (auto cfd : calledFuncDetail)
          {
              LOGDEBUG << "calledFuncDetail  cfd : " << cfd << std::endl;
              tempText += "\n " + cfd + ";";
          }
      }
  
      tempText += codetext.substr(pos2 - 1);
      if (isSaveToFile)
      {
        //fs::path tempHeaderPath = randomDir / outHeaderPath;
        std::ofstream outHeader(outHeaderPath);
        outHeader << tempText;
        outHeader.close();
      }

      LOGDEBUG << "tempText  end :  " << tempText << std::endl;
      return tempText;
    }
  }
  else
  {
    throw Exception() << ErrStr("contract class isn't in the main contract header file,please define it in main contract header file.") << ErrStr(strerror(errno));
  }
  return codetext;
}

void createJsonAbi(const ABIDef &abiDef, const ContractDef &contractDef,
                   const std::string &srcFile, const std::string &destPath,
                   fs::path &randomDir)
{
  fs::path current = fs::current_path();
  fs::path outPath(destPath);

  if (srcFile.empty())
  {
    throw Exception() << ErrStr("src name is empty");
  }
  string fileName = srcFile + ".abi.json";
  if (destPath.empty())
  {
    outPath = current / fileName;
  }
  fs::path tmpFile = randomDir / fileName;

  LOGERROR << "random:[" << randomDir.string() << "] fileName:" << fileName;

  LOGDEBUG << "tmpFile:[" << tmpFile.string() << "]";

  std::ofstream fs(tmpFile.string().c_str());

  if (!fs.is_open())
  {
    throw Exception() << ErrStr("fs is not open:") << ErrStr(strerror(errno));
  }
  outputJsonABI(abiDef, contractDef, fs);
  fs.close();

  LOGDEBUG << "copy ::" << tmpFile.string() << " to ::" << outPath.string();
  fs::copy_file(tmpFile, outPath, fs::copy_option::overwrite_if_exists);
}

void createExportsFile(const ABIDef &abiDef, const string &srcFile,
                       const string &destPath, fs::path &randomDir)
{
  fs::path current = fs::current_path();
  fs::path outPath(destPath);

  if (srcFile.empty())
  {
    throw Exception() << ErrStr("src name is empty");
  }
  string fileName = srcFile + ".exports";
  if (destPath.empty())
  {
    outPath = current / fileName;
  }
  fs::path tmpFile = randomDir / fileName;

  LOGERROR << "random:[" << randomDir.string() << "] fileName:" << fileName;

  LOGDEBUG << "tmpFile:[" << tmpFile.string() << "]";

  std::ofstream fs(tmpFile.string().c_str());

  if (!fs.is_open())
  {
    throw Exception() << ErrStr("fs is not open:") << ErrStr(strerror(errno));
  }

  for (size_t i = 0; i < abiDef.abis.size(); i++)
  {
    fs << abiDef.abis[i].methodName << std::endl;
  }
  fs.close();

  LOGDEBUG << "copy ::" << tmpFile.string() << " to ::" << outPath.string();
  fs::copy_file(tmpFile, outPath, fs::copy_option::overwrite_if_exists);
}

void createContractFile(fs::path &randomDir, const string &srcPath,
                        const string &filename, const string &externC,
                        const std::string &abidef_output, const ABIDef &abidef, const string &contractName)
{
  llvm::SmallString<64> res;
  llvm::sys::path::system_temp_directory(true, res);

  std::string abidef_filename(filename);
  std::string abidef_filename_temp(filename);
  std::string headerStr;
  if (!abidef_output.empty())
  {
    llvm::SmallString<256> fn = llvm::sys::path::filename(abidef_output);
    abidef_filename = std::string(fn.str());
    abidef_filename_temp = std::string(fn.str()) + "temp";
  }

  LOGDEBUG << "abidef_output " << abidef_output << " abidef_filename "  << abidef_filename << std::endl;
  fs::path tmpFile = randomDir / abidef_filename;
  std::ofstream tmpStream(tmpFile.string().c_str());
  if (!tmpStream.is_open())
  {
    throw Exception() << ErrStr("tmp src file is not open:")
                      << ErrStr(strerror(errno));
  }

  std::string src(srcPath);
  if (!abidef_output.empty())
  {
    src = abidef_output;
  }
  std::string hppPath;
  std::ifstream hppStream;
  std::ifstream srcStream(src);
  vector<std::string> calledFuncDetail;
  if (!srcStream.is_open())
  {
    throw Exception() << ErrStr("src is not open:") << ErrStr(strerror(errno));
  }

  std::string macrostr((std::istreambuf_iterator<char>(srcStream)),
                       std::istreambuf_iterator<char>());

  //string macrostr = buffer;
  FileStatistics fs;
  LOGDEBUG << "macrostr  =======::" << macrostr << std::endl;
  LOGDEBUG << "headerStr  =======::" << headerStr << std::endl;

  string removedComments = RemoveCommentAndStatistics(macrostr, fs, false, false);
  LOGDEBUG << "removedComments  =======::" << removedComments << std::endl;
  removedComments = GetAndInsertBeCalled(removedComments, abidef, calledFuncDetail, contractName);
  LOGDEBUG << "removedComments end  =======::" << removedComments << std::endl;

  // handling header file issues
  std::string str = "class\\s*" + contractName + "\\s*\\{";
  LOGDEBUG << "str ============";
  LOGDEBUG << "str ============" << str;
  //regex findClass(R"(class\s*)" + contractName + R"(\s*\{)");
  regex findClass(R"(class\s*)" + contractName + R"(\s*(.+?)\{)");
  // regex findClass(str);
  // regex findClass("");
  LOGDEBUG << "findClass end  =======::" << std::endl ;

  smatch claSma;
  LOGDEBUG << "claSma end  =======::" << std::endl;

  // indicate the header file and the implementation file are in the same file
  if (regex_search(removedComments, claSma, findClass))
  {
    LOGDEBUG << "hppPath i am here :  " << hppPath ;
    LOGDEBUG << "hppPath i am here :  " << hppPath << std::endl;
    //headerStr = removedComments;
    removedComments = InsertFuncToHeaderFile(removedComments, calledFuncDetail, abidef, filename, randomDir, contractName, "", false);
    CompareHeaderAndImplFileMacro(removedComments,abidef, contractName, true);
  }
  else
  {
    CompareHeaderAndImplFileMacro(removedComments,abidef, contractName, false);
    regex nameReg("\\.cpp");
    hppPath = regex_replace(src, nameReg, ".hpp");
    LOGDEBUG << "hppPath  :  " << hppPath;
    hppStream = std::ifstream(hppPath);
    std::string hppOutPathTemp(std::string(res.c_str()) + "/" + abidef_filename);
    std::string hppOutPath = regex_replace(hppOutPathTemp, nameReg, "temp.hpp");
    LOGDEBUG << "hppOutPath  :  " << hppOutPath;
    if (!hppStream.is_open())
    {
      throw Exception() << ErrStr("can't find the main header file,please name it same with the main contract file.") << ErrStr(strerror(errno));
    }
    headerStr = std::string((std::istreambuf_iterator<char>(hppStream)),
                            std::istreambuf_iterator<char>());
    //LOGDEBUG << "headerStr ======== : " << headerStr << std::endl;
    LOGDEBUG << "abaabaab ======== : ";
    LOGDEBUG << "contractName ======== : " << contractName;

    LOGDEBUG << "randomDir ======== : " << randomDir.string();
    LOGDEBUG << "hppOutPath ======== : " << hppOutPath;
    for (auto fu : calledFuncDetail){
        LOGDEBUG << "fu ======== : " << fu;
    } 

    InsertFuncToHeaderFile(headerStr, calledFuncDetail, abidef, filename, randomDir, contractName, hppOutPath, true);
    std::string fileTempName = filename.substr(0, filename.find_last_of(".")) + "temp.hpp";
    regex replaceInclude(R"(#include\s+\")" + regex_replace(filename, nameReg, ".hpp"));
    removedComments = regex_replace(removedComments, replaceInclude, R"(#include ")" + fileTempName);
    LOGDEBUG << "headerStr removedComments  ======== : " << removedComments << std::endl;
  }
  fs::path tmpFileSrc = randomDir / abidef_filename_temp;
  std::fstream tmpSrcStream(tmpFileSrc.string().c_str());
  if (!tmpSrcStream.is_open())
  {
    throw Exception() << ErrStr("tmpSrcStream is not open:") << ErrStr(strerror(errno));
  }
  tmpSrcStream << removedComments;
  tmpSrcStream.seekg(0, std::ios::beg);
  tmpSrcStream.close();
  std::fstream tmpSrcStreamNew(tmpFileSrc.string().c_str());

  string line;
  bool skip = false;
  while (getline(tmpSrcStreamNew, line))
  {
    if (line.find("dipc autogen end") != string::npos)
    {
      skip = false;
      continue;
    }
    if (line.find("dipc autogen begin") != string::npos)
    {
      skip = true;
      continue;
    }
    if (!skip)
    {
      tmpStream << line << "\n";
    }
    line.clear();
  }

  tmpStream << externC;
  tmpStream.close();
  srcStream.close();
  tmpSrcStreamNew.close();

  LOGDEBUG << "swap src::" << srcPath << " tmp::" << tmpFile.string();
  // fs::remove(srcPath);

  std::string dst(std::string(res.c_str()) + "/" + abidef_filename);
  LOGDEBUG << R"(std::string(res.c_str()) + "/" + abidef_filename      )" << std::string(res.c_str()) + "/" + abidef_filename << std::endl;
  fs::copy_file(tmpFile, dst, fs::copy_option::overwrite_if_exists);
}
void prepareFile(const string &filename)
{
  fstream fs(filename);

  string line;

  auto cur = fs.tellg(); // tellg()  Used for the input stream, returning the current position of the ‘get�? pointer in the stream

  //    auto begin = fstream::pos_type(-1);
  //    auto end = fstream::pos_type(-1);

  vector<tuple<fstream::pos_type, string>> replaceText; // The tuple in C++ is similar to the python language. It is a powerful container that allows multiple different types of data to be stored. It is a generalization of pairs.
  string tailText;
  bool skip = false;
  bool append = false;
  while (getline(fs, line))
  {
    if (append)
    {
      tailText.append(line);
    }
    // delete comments

    if (line.find("dipc autogen end") != string::npos)
    {
      // end = cur + fstream::pos_type(line.length());
      replaceText.push_back(tuple<fstream::pos_type, string>(cur, line));
      skip = false;
      append = true;
    }
    if (line.find("dipc autogen begin") != string::npos)
    {
      // begin = cur;
      skip = true;
    }
    if (skip)
    {
      replaceText.push_back(tuple<fstream::pos_type, string>(cur, line));
    }

    line.clear();
    cur = fs.tellg();
  }

  if (replaceText.empty())
  {
    return;
  };
  fs.close();

  fstream writeAnno(filename);
  if (!writeAnno.is_open())
  {
    throw Exception() << ErrStr("open file failed");
  }
  string rt = "//";
  writeAnno.seekp(get<0>(replaceText[0])); // seekp：Set the file stream pointer position of the output file stream   get<i>: get the value of the i-th element
  for (int i = 0; i < replaceText.size(); i++)
  {
    writeAnno << rt << get<1>(replaceText[i]) << '\n';
  }
  writeAnno << tailText;
  //    writeAnno.seekp(end);
  //    writeAnno << "*/";
  //    writeAnno.seekp(begin);
  //    writeAnno << "/*//dipc autogen begin";
  writeAnno.close();
}
int main(int argc, const char **argv)
{
  llvm::cl::SetVersionPrinter([](llvm::raw_ostream &os) {
    os << "dipc-abigen version "
       << "${VERSION_FULL}"
       << "\n";
  });
  try
  {
    // cl::ParseCommandLineOptions(
    //   argc, argv, " Dipc-abi -> WebAssembly compiler");
    tooling::CommonOptionsParser op(argc, argv, abiGeneratorOptions);
    

    if (op.getSourcePathList().size() > 1)
    {
      // throw Exception() << ErrStr("only one contract file");
      return -1;
    }
    std::cout << R"(main contract file )" << op.getSourcePathList()[0] << std::endl;
    string srcPath = op.getSourcePathList()[0];
    prepareFile(op.getSourcePathList()[0]);

    tooling::ClangTool Tool(op.getCompilations(), op.getSourcePathList());

    llvm::SmallString<64> res;
    llvm::sys::path::system_temp_directory(true, res);
    fs::path randomDir(std::string(res.c_str()));
    randomDir = randomDir /
                randomString(20)
                    .c_str(); // "57298a3ci7d0g504ha00";// randomString(20);

//         //std::cout << project_name << " logDir    " << randomDir << std::endl;
//         if (!fs::create_directories(randomDir))
//         {
//         throw Exception() << ErrStr("create dir failed:")
//                             << ErrStr(strerror(errno));
//         }

    std::cout << "verbose  value "  <<  verbose  << std::endl;
    toInitLog("dipc-abi", verbose,logPath,logLevel,randomDir);

    LOGDEBUG << "dipc-abi  verbose  "  << verbose << std::endl;
    LOGDEBUG << "dipc-abi argv  start ...."  << std::endl;
    for(int i = 0; argv[i] ; i++){
      LOGDEBUG << "  " << argv[i];
    }
    LOGDEBUG << "  dipc-abi argv end "  << std::endl;
    

    vector<string> action;
    ContractDef contractDef;

    LOGDEBUG << "start run";
    int result = Tool.run(CreateFindMacroFactory(contractDef, action).get());

    if (result != 0 || contractDef.name.empty())
    {
      LOGDEBUG << "result :" << result << "contractDef.name : " << contractDef.name ;
      throw Exception() << ErrStr("find macro failed");
      return -1;
    }

    LOGDEBUG << "result:" << result << "contract fullname:["
             << contractDef.fullName << "]";

    // for (const string &act : action)
    // {
    //   LOGDEBUG << "contract:" << contractDef.name << "  action:" << act;
    // }

    ABIDef abiDef;
    result = Tool.run(createFactory(contractDef.name, action, abiDef).get());

    if (result != 0)
    {
      // throw Exception() << ErrStr("find contract abi defined failed");
      return -1;
    }

    LOGINFO << "find method success"
            << "find abi size:" << abiDef.abis.size();
    bool foundInit = false;
    for (size_t i = 0; i < abiDef.abis.size(); ++i)
    {
      LOGDEBUG << "methodName:" << abiDef.abis[i].methodName << " args:(";
      if (abiDef.abis[i].methodName == "init")
      {
        foundInit = true;
      }
      for (size_t j = 0; j < abiDef.abis[i].args.size(); ++j)
      {
        LOGDEBUG << "name:" << abiDef.abis[i].args[j]
                 << ", typeName:" << abiDef.abis[i].types[j].typeName
                 << " realTypeName:" << abiDef.abis[i].types[j].realTypeName;
      }
      LOGDEBUG << ")";
      LOGDEBUG << "return typeName:" << abiDef.abis[i].returnType.typeName
               << " realTypeName:" << abiDef.abis[i].returnType.realTypeName;
    }

    if (!foundInit)
    {
      std::cerr
          << "ERROR: <dipc-abigen> `init` function not found!!! Please use "
             "`EXPORT` declare it."
          << std::endl;
      return -1;
    }

    LOGDEBUG << "start create abi json";

    string srcFilename =
        fs::path(op.getSourcePathList()[0]).filename().string();

    createJsonAbi(abiDef, contractDef, srcFilename, abigen_output_opt,
                  randomDir);
    createExportsFile(abiDef, srcFilename, exports_output_opt, randomDir);

    string externC = generateAbiCPlusPlus(contractDef, abiDef);
    createContractFile(randomDir, op.getSourcePathList()[0], srcFilename,
                       externC, abidef_output_opt, abiDef, contractDef.fullName);

  }
  catch (Exception e)
  {
    cerr << *boost::get_error_info<ErrStr>(e) << endl;
    cerr << e.what() << endl;
    return -1;
  }

     
  return 0;
}
