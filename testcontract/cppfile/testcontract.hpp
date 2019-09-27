#include "libraries/dipclib/cpplib/contract.hpp"
#include <map>
#include <string>

using namespace dipperlib;

CONTRACT testContract:public contract{
    public: 

    //构造函数
    using contract::contract;
    testContract(Address ownerAddr, Address contractAddr,std::string name,std::vector<char> data,std::map<std::string,std::string> mapData):contract(ownerAddr,contractAddr,name,data),testMapData(mapData){}
    
    //合约测试方法
    void sayHelloWorld();

    //保存key value 到合约map
    void saveKeyValue(std::string key,std::string value);

    //从合约map中获取key对应的value值
    std::string getKeyValue(std::string key);

    protected:
    std::map<std::string,std::string> testMapData;    
};
