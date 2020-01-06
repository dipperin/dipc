#include <dipc/dipc.hpp>
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include <map>

using namespace dipc;

struct Order {
        std::string orderId;
        uint16_t orderType;     // 1 means buy, 0 means sell
        std::string stockCode;
        uint64_t stockPrice;
        uint64_t stockAmount;
        uint64_t endTime;
        uint16_t orderStatus;   // 0 mean unsettled, 1 mean settled, 2 cancel, 3 expire
        std::string toJson(){
            std::string result = "{ \"orderId\":\"" + orderId + "\",\"orderType\":" 
                + std::to_string(orderType)+ ",\"stockCode\":\""
                + stockCode+"\",\"stockPrice\":"+ std::to_string(stockPrice)
                +",\"stockAmount\":"+ std::to_string(stockAmount)
                + ",\"endTime\":"+std::to_string(endTime)
                + ",\"orderStatus\":"+std::to_string(orderStatus)+"}";
            return result;
        }
};

 class PortfolioInfo {
        private:
        // char*  portfolioName;
        // char*  portfolioDesc;
        // char*  portfolioOwnerAddr;
        std::string portfolioName;
        std::string  portfolioDesc;
        std::string portfolioOwnerAddr;
        std::string portfolioResult;
        uint64_t portfolioBalance;
        //uint64_t orderRecordNum;
        //static uint64_t ordersLen;
        //std::map<char*, Order> orders;
        std::map<std::string, struct Order> orders;
        //Order orders[10];
        public:
        //PortfolioInfo(char* _portfolioName, char* _portfolioDesc, char* _portfolioOwnerAddr, uint64_t _portfolioBalance):portfolioName(_portfolioName),portfolioDesc(_portfolioDesc), portfolioOwnerAddr(_portfolioOwnerAddr), portfolioBalance(_portfolioBalance){
        PortfolioInfo(){};
        PortfolioInfo(char* _portfolioName, char* _portfolioDesc, char* _portfolioOwnerAddr):portfolioName(_portfolioName),portfolioDesc(_portfolioDesc), portfolioOwnerAddr(_portfolioOwnerAddr) {

        };
        // std::string toJson(){
        //     rapidjson::StringBuffer strBuf;
        //     rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);
        //     writer.StartObject();
        //     writer.Key("portfolioName");
        //     writer.String(portfolioName.data());
        //     writer.Key("portfolioDesc");
        //     writer.String(portfolioDesc.data());
        //     writer.Key("portfolioOwnerAddr");
        //     writer.String(portfolioOwnerAddr.data());
        //     writer.Key("portfolioBalance");
        //     writer.Uint64(portfolioBalance);
        //     writer.Key("orders");
        //     writer.StartArray();
        //     //std::map<char* , Order>::iterator iter;
        //     std::map<std::string , Order>::iterator iter;
        //     iter = orders.begin();
        //     while(iter != orders.end()){
        //         Order order = iter->second;
        //         writer.StartObject();
        //         writer.Key("orderId");
        //         writer.String(order.orderId.data());
        //         writer.EndObject();
        //     }
        //     writer.EndArray();
        //     writer.EndObject();

        //     print(strBuf.GetLength());
        //     //strBuf.Flush();
        //     std::string result = strBuf.GetString();
        //     auto ch = strBuf.GetString();
        //     print(*ch);
        //     if(*ch != '\0'){
        //         ch++;
        //         print(ch);
        //     }

        //     portfolioResult = strBuf.GetString();
        //     //strBuf.Flush();
        //     // char re[] = strBuf.GetString();
        //     // copy(strBuf.GetString(), result);
        //     print(result);
        //     print(portfolioResult);
        //     return result;
        // }

         std::string toJson(){
             std::string result;
            //result += "{ \"portfolioName\":" + portfolioName +  ",\"portfolioDesc\":" + portfolioDesc + ",\"portfolioOwnerAddr\":" + portfolioOwnerAddr+ ",\"orders\":[";
            result += "{ \"portfolioName\":\"" + portfolioName +  "\",\"portfolioDesc\":\"" + portfolioDesc + "\",\"portfolioOwnerAddr\":\"" + portfolioOwnerAddr+ "\",\"orders\":[";
            std::map<std::string , Order>::iterator iter;
            iter = orders.begin();
            bool hasOrder = false;
            while(iter != orders.end()){
                print("get order info");
                Order order = iter->second;
                result += "{ \"orderId\":\"" + order.orderId + "\",\"orderType\":" 
                + std::to_string(order.orderType)+ ",\"stockCode\":\""
                + order.stockCode+"\",\"stockPrice\":"+ std::to_string(order.stockPrice)
                +",\"stockAmount\":"+ std::to_string(order.stockAmount)
                + ",\"endTime\":"+std::to_string(order.endTime)
                + ",\"orderStatus\":"+std::to_string(order.orderStatus)+"},";
                hasOrder = true;
                iter++;
            }
            if(hasOrder){
                result = result.substr(0,result.size()-1);
            }
            print("get result info");
            result += "]}";
            print(result);
            //print(portfolioResult);
            return result;
        }
        void addOrder(Order &order){
            auto localOrder = order;
            print("addOrder  start");
            print(order.orderId);
            //orders.insert(std::pair<std::string, Order>(order.orderId, localOrder));
            orders[order.orderId] = order;
        }
        void parseJson(std::string info){
            rapidjson::Document doc;
            print("parseJson start ");
            print(info);
            if(!doc.Parse(info.data()).HasParseError()){
   
                if(doc.HasMember("portfolioName") && doc["portfolioName"].IsString())
                {   
                    print("portfolioName");
                    portfolioName = doc["portfolioName"].GetString();
                }
      
                if(doc.HasMember("portfolioDesc") && doc["portfolioDesc"].IsString())
                {
                    portfolioDesc = doc["portfolioDesc"].GetString();
                }
                if(doc.HasMember("portfolioOwnerAddr") && doc["portfolioOwnerAddr"].IsString())
                {
                    portfolioOwnerAddr = doc["portfolioOwnerAddr"].GetString();
                }
    
                if(doc.HasMember("portfolioBalance") && doc["portfolioBalance"].IsUint64())
                {
                portfolioBalance = doc["portfolioBalance"].GetUint64();
                }
                print("start to parse orders");
                if(doc.HasMember("orders") && doc["orders"].IsArray()){
                    print("start to parse orders  one ");

                    const rapidjson::Value& ors = doc["orders"];
                    size_t len = ors.Size();
                    for(size_t i = 0; i < len; i++)
                    {
                    print("start to parse orders  two ");

                        struct Order orderPar;
                        const rapidjson::Value& ord = ors[i];
                        if(ord.HasMember("orderId") && ord["orderId"].IsString())
                        {
                            orderPar.orderId = ord["orderId"].GetString();
                        }

                        if(ord.HasMember("orderType") && ord["orderType"].IsUint())
                        {
                            orderPar.orderType = ord["orderType"].GetUint();
                        } 
                         if(ord.HasMember("stockCode") && ord["stockCode"].IsString())
                        {
                            orderPar.stockCode = ord["stockCode"].GetString();
                        } 
                         if(ord.HasMember("stockPrice") && ord["stockPrice"].IsUint64())
                        {
                            orderPar.stockPrice = ord["stockPrice"].GetUint64();
                        } 
                        if(ord.HasMember("stockAmount") && ord["stockAmount"].IsUint64())
                        {
                            orderPar.stockAmount = ord["stockAmount"].GetUint64();
                        } 
                        if(ord.HasMember("endTime") && ord["endTime"].IsUint64())
                        {
                            orderPar.endTime = ord["endTime"].GetUint64();
                        } 
                        if(ord.HasMember("orderStatus") && ord["orderStatus"].IsUint())
                        {
                            orderPar.orderStatus = ord["orderStatus"].GetUint();
                        }
                        print("orderPar");
                        print(orderPar.orderId);
                        addOrder(orderPar);
                        //orders.insert(std::pair<std::string, Order>(orderPar.orderId, orderPar));
                    }
                }
        
            } else {
                print("parseJson error ");
            }
        }
    

        Order getOrder(char * orderId){
            return orders[orderId];
        }

        void setOrderStatus(char* orderId, uint16_t orderStatus){
            orders[orderId].orderStatus = orderStatus;
        }
        std::string getOwnerAddress(){
        //char* getOwnerAddress(){
            return portfolioOwnerAddr;
        }

        std::string getPortfolioResult(){
        //char* getOwnerAddress(){
            return portfolioResult;
        }
      
};




char ownc[] = "own";
char ownerTotalTokenc[] = "ownTotal";
char changeRatec[] = "changeRate";
char userCanWithdrawc[] = "userWithdraw";
char totalTokenc[] = "totalToken";
char portfolioMapc[] = "portfolioMap";
char portfolioBalancec[] = "portfolioBalance";
char printInfoc[] = "printInfo";
class PortfolioManage : public Contract {
private:
    String<ownc> owner;
    Uint64<ownerTotalTokenc> ownerTotalToken;
    Uint64<changeRatec> changeRate;
    Bool<userCanWithdrawc> userCanWithdraw;
    Uint64<totalTokenc> totalToken; 
    //Map<portfolioMapc, char * , std::string> portfolioStorge;
    Map<portfolioMapc, std::string , std::string> portfolioStorge;
    Map<portfolioBalancec, std::string , uint64_t> portfolioBalance; 
    Bool<printInfoc> printInfo;
    void isOwner(){
        print("start find owner");
        std::string callerStr = caller().toString();
        print(callerStr);
        printWithContral(&callerStr[0], printInfo.get());
        std::string ownerStr = owner.get();
        print(ownerStr);
        printWithContral(&ownerStr[0], printInfo.get());
        
        DipcAssert(callerStr == owner.get());
        printWithContral("isOwn", printInfo.get());
    }
    
    inline uint64_t getTotalBalance(){
        print("callValueUDIP");
        print(dipc::callValueUDIP());
        print(changeRate.get());
        //print(uint64_t(std::pow(10.0,3)));
       return dipc::callValueUDIP() * changeRate.get() / 1000;
    }
   

public:
    PAYABLE void init();
    EXPORT void setChangeRate(uint64_t _changeRate);
    EXPORT void setUserCanWithdraw(bool _canWithdraw);
    PAYABLE void createPortfolio(char* _portfolioName, char* _portfolioDesc);
    PAYABLE void depositPortfolio(char* _portfolioName);
    PAYABLE void createOrder(char* _portfolioName, char* _orderId, uint16_t _orderType, char* _stockCode, uint64_t _stockPrice, uint64_t _stockAmount, uint64_t _endTime);
    EXPORT void dealOrder(char* _portfolioName, char* _orderId);
    EXPORT void revocationOrder(char* _portfolioName, char* _orderId);
    EXPORT void withdrawPortfolio(char* _portfolioName, uint64_t _withdrawBalance);
    EXPORT void withdrawPool(uint64_t _balance);
    PAYABLE void depositPool();
    CONSTANT uint64_t queryTotalToken();
    CONSTANT uint64_t queryPortfolioBalance(char* _portfolioName);
    CONSTANT bool queryPortfolioNameIfUsed(char* _portfolioName);
    CONSTANT uint64_t queryChangeRate();
};
DIPC_EVENT(initEvent, const char* ,uint64_t);
DIPC_EVENT(tokenCount, const char* ,uint64_t, const char*, uint64_t);
DIPC_EVENT(portfolioInfo, const char*, const char* ,const char* , uint64_t);
DIPC_EVENT(createOrder, const char*, const char*, const char*, const char*, const char*, uint64_t);
DIPC_EVENT(depositPortfolio, const char*, const char*, const char*, uint64_t);
DIPC_EVENT(withdrawPortfolio, const char*, const char*, const char*, uint64_t);
DIPC_EVENT(revocationPortfolio, const char*, const char*, const char*, const char*);
DIPC_EVENT(dealPortfolio, const char*, const char*, const char*, const char*);
DIPC_EVENT(ErrEvent, const char*, const char*);