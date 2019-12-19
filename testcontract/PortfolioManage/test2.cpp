#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include <map>
#include <string>
#include <iostream>

struct Order {
    //private:
        //char* orderId;
        std::string orderId;
        uint16_t orderType;     // 1 means buy, 0 means sell
        char* stockCode;
        uint64_t stockPrice;
        uint64_t stockAmount;
        uint64_t endTime;
        uint16_t orderStatus;   // 0 mean unsettled, 1 mean settled, 2 cancel, 3 expire
        std::string toJson(){
            return "";
        }
    // public:
    //     Order(char* _orderId, uint8_t _orderType):orderId(_orderId), orderType(_orderType){};
    //     std::string toJson(){
    //         rapidjson::StringBuffer strBuf;
    //         rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);
    //         writer.Key("");

    //     }
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
        std::map<std::string, Order> orders;
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
            //rapidjson::StringBuffer strBuf;
            //rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);
            result += "{ \"portfolioName\":\"" + portfolioName +  "\",\"portfolioDesc\":\"" + portfolioDesc + "\",\"portfolioOwnerAddr\":\"" + portfolioOwnerAddr+ "\",\"orders\":[";
            //writer.StartObject();
            //writer.Key("portfolioName");
            //writer.String(portfolioName.data());
            // writer.Key("portfolioDesc");
            // writer.String(portfolioDesc.data());
            // writer.Key("portfolioOwnerAddr");
            // writer.String(portfolioOwnerAddr.data());
            // writer.Key("portfolioBalance");
            // writer.Uint64(portfolioBalance);
            // writer.Key("orders");
            // writer.StartArray();
            //std::map<char* , Order>::iterator iter;
            std::map<std::string , Order>::iterator iter;
            iter = orders.begin();
            while(iter != orders.end()){
                Order order = iter->second;
                // writer.StartObject();
                result += "{ \"orderId\":\"" + order.orderId + "\"},";
                //writer.Key("orderId");
                //writer.String(order.orderId.data());
                //writer.EndObject();
            }
            result.substr(0,result.size()-1);
            result += "]}";
            // writer.EndArray();
            // writer.EndObject();

            //print(strBuf.GetLength());
            //strBuf.Flush();
            //std::string result = strBuf.GetString();
            // auto ch = strBuf.GetString();
            // print(*ch);
            // if(*ch != '\0'){
            //     ch++;
            //     print(ch);
            // }

            //portfolioResult = strBuf.GetString();
            //strBuf.Flush();
            // char re[] = strBuf.GetString();
            // copy(strBuf.GetString(), result);
            std::cout << result  << std::endl;
            //print(portfolioResult);
            return result;
        }
        void parseJson(std::string info){
            rapidjson::Document doc;
            std::cout << "parseJson start " << std::endl;
            if(!doc.Parse(info.data()).HasParseError()){
   
                if(doc.HasMember("portfolioName") && doc["portfolioName"].IsString())
                {   
                    std::cout <<  "portfolioName"  << std::endl;
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
        
            } else {
                std::cout << "parseJson error "  << std::endl;
            }
        }
        void addOrder(Order order){
            orders.insert(std::pair<std::string, Order>(order.orderId, order));
        }

        Order getOrder(char * orderId){
            return orders[orderId];
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


int main(){
    PortfolioInfo info("name", "desc", "addr");
    std::string str = "{ \"portfolioName\": \"winner\", \"portfolioDesc\": \"winner\",\"portfolioOwnerAddr\":\"0x0000E114Ae14dc5F4B30A1df81B12c9099D67843963F\", \"orders\":[]}";
    std::cout <<  info.toJson()  << std::endl;
    str.substr(0,str.size()+1);
    std::cout << str << std::endl;
    //info.parseJson(str);
}