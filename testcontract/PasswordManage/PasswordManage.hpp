#include "dipc/dipc.hpp"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
using namespace dipc;

class PasswordInfo {
public:
    std::string passwordName;
    std::string toAddr;
    uint64_t balance;
    uint64_t timeRegistered;
    PasswordInfo(){};
    PasswordInfo(char* _passwordName, char* _toAddr, uint64_t _balance, uint64_t _timeRegistered):passwordName(_passwordName), 
        toAddr(_toAddr),balance(_balance), timeRegistered(_timeRegistered){ }
    std::string toJson(){
        // neb::CJsonObject oJson;
        // oJson.Add("passwordName", passwordName );
        // oJson.Add("toAddr", toAddr);
        // oJson.Add("balance", balance);
        // return oJson.ToString();
            print("timestamp");
            print(std::to_string(timeRegistered));
            std::string result = "{ \"passwordName\":\"" + passwordName + "\",\"toAddr\":\"" 
                + toAddr+ "\",\"balance\":" + std::to_string(balance)
                +",\"timeRegistered\":"+ std::to_string(timeRegistered) + "}";
            return result;
    }

    void parseJson(std::string info){
        // neb::CJsonObject oJson;
        // std::string pwName;
        // std::string toAddrStr;
        // if(oJson.Parse(pInfo)){
        //     oJson.Get("passwordName", pwName);
        //     oJson.Get("toAddr", toAddrStr);
        //     oJson.Get("balance", balance);
        // }
        // passwordName = &pwName[0];
        // toAddr = &toAddrStr[0];
            rapidjson::Document doc;
            print("parseJson start ");
            if(!doc.Parse(info.data()).HasParseError()){
   
                if(doc.HasMember("passwordName") && doc["passwordName"].IsString())
                {   
                    print("passwordName");
                    passwordName = &(doc["passwordName"].GetString()[0]);
                }
      
                if(doc.HasMember("toAddr") && doc["toAddr"].IsString())
                {
                    toAddr = &(doc["toAddr"].GetString()[0]);
                }
    
                if(doc.HasMember("balance") && doc["balance"].IsUint64())
                {
                     balance = doc["balance"].GetUint64();
                }

                if(doc.HasMember("timeRegistered") && doc["timeRegistered"].IsUint64())
                {
                     timeRegistered = doc["timeRegistered"].GetUint64();
                }
        
            } else {
                print("parseJson error ");
            }
    }
};

char expireTimec[] = "expireTime";
char passwordStorec[] = "passwordStore";
char passwordStoreHisc[] = "passwordStoreHis";
char ownerc[] = "owner";
class PasswordManage : public Contract {
private:
    Uint64<expireTimec> expireTime;
    Map<passwordStorec, std::string, std::string> passwordStore;
    Map<passwordStoreHisc, std::string, std::string> passwordStoreHis;
    String<ownerc> owner;

    void isOwner(){
        std::string callerStr = caller().toString();
        std::string ownerStr = owner.get();
        DipcAssert(callerStr == owner.get());
    }

public:
   PAYABLE void init();
   EXPORT void registerPassword(char* passwordName, uint64_t _balance);
   CONSTANT char* queryExistPasswords();
   CONSTANT bool queryPasswordIsUsed(char* passwordName);
   PAYABLE void payByPassword(char* passwordName);
   PAYABLE void depositeContract();
   EXPORT void setExpireTime(uint64_t _expireTime);
};

DIPC_EVENT(expireTimeSetted,uint64_t);
DIPC_EVENT(depositingValue,uint64_t);
DIPC_EVENT(ErrEvent, const char*, const char*);
DIPC_EVENT(registerPasswordEvent, const char*, const char*, const char*);
DIPC_EVENT(payByPasswordEvent, const char*, uint64_t);


