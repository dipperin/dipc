#include "testcontract.hpp"
#include "libraries/dipclib/clib/print.h"

//testContract 提供的合约方法

void testContract::sayHelloWorld(){
    prints("hello world");
}

void testContract::saveKeyValue(std::string key,std::string value){
    using namespace std;
    pair<map<string, string>::iterator, bool> Insert_Pair; 
    Insert_Pair = testMapData.insert(pair<string,string>(key,value));
    
    if (Insert_Pair.second){
        prints("Insert Successfully");
    } else {
        prints("Insert fail");
    } 
    return ;
}

std::string testContract::getKeyValue(std::string key){
    using namespace std;

    map<string,string>:: iterator iter;
    iter = testMapData.find(key);
    if (iter != testMapData.end()){
        return iter->second ;
    } else {
        prints("not find the key in the testMapData");
        return "" ;
    }
}

//合约调用入口
extern "C" {
    /**
     * init为初始化函数，当合约上传或更新的时候执行。
     */
    void init()  {
       prints( "Init testContract!" );
    }
} // extern "C"







