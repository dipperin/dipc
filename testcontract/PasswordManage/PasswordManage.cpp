#include "PasswordManage.hpp"

/**
 * @brief init method used to setup own addr
 */ 
PAYABLE void PasswordManage::init(){
    std::string ownerStr = caller().toString();
    *owner = ownerStr;
}
/**
 * @brief registerPassword method used to register a password
 * @param passwordName: password name
 * @param _balance:     the beneficiary balance, _balance unit is DIP
 */ 
EXPORT void PasswordManage::registerPassword(char* passwordName, uint64_t _balance){
    std::string toAddr = caller().toString();
    PasswordInfo info(passwordName, &toAddr[0], _balance, dipc::timestamp());
    std::string pInfo = info.toJson();
    if (passwordStore.get()[passwordName] != "") {
        DIPC_EMIT_EVENT(ErrEvent, "registerPassword  password exist", passwordName);
        return;
    }
    (*passwordStore)[passwordName] = pInfo;
    print("registerPassword pInfo");
    print(pInfo);
    std::string callerStr = dipc::caller().toString();
    DIPC_EMIT_EVENT(registerPasswordEvent, &callerStr[0], passwordName, &pInfo[0]);
}
/**
 * @brief  queryExistPasswords used to query to password name had been used
 * @return the array of password names had been used
 */ 
CONSTANT char* PasswordManage::queryExistPasswords(){
    std::map<std::string, std::string>::iterator iter;
    auto mapStore = passwordStore.get();
    iter = mapStore.begin();
    std::string result = "[";
    print("queryExistPasswords start");
    while(iter != mapStore.end()){
        result += iter->first + ","; 
        iter++;
    }
    print("queryExistPasswords end");
    if(!mapStore.empty()){
       result.substr(0,result.size()-1);
    }
    result += "]";
    print("queryExistPasswords  result");
    print(result);
    return &result[0];
}
/**
 * @brief queryPasswordIsUsed used to query password is used or not
 * @param passwordName: the password name
 * @return true is used, false is not used
 */ 
CONSTANT bool PasswordManage::queryPasswordIsUsed(char* passwordName){
     if (passwordStore.get()[passwordName] != "") {
        return true;
    }
    return false;
}
/**
 * @brief payByPassword is used to pay to the password's owner
 * @param passwordName: the password name
 */ 
PAYABLE void PasswordManage::payByPassword(char* passwordName){
    std::string pInfo = passwordStore.get()[passwordName];
    PasswordInfo pwInfo;
    pwInfo.parseJson(pInfo);
    print("pwInfo");
    print(pwInfo.toJson());
    uint64_t transferValue = dipc::callValueUDIP();
    print("transferValue");
    print(transferValue);
    print(pwInfo.balance);
    if(dipc::timestamp() >=  expireTime.get() + pwInfo.timeRegistered){
        DipcAssert(transferValue / 1000 == pwInfo.balance);
        Address toAddr = Address(pwInfo.toAddr,22);
        callTransferUDIP(toAddr, transferValue);
        DIPC_EMIT_EVENT(payByPasswordEvent, passwordName,  transferValue * 1000);
    } else {
        DIPC_EMIT_EVENT(ErrEvent, "payByPassword  password exist", passwordName);
    }
    //DipcAssert( );
    (*passwordStore).erase(passwordName);
    (*passwordStoreHis)[std::string(passwordName)+"_"+pwInfo.toAddr+"_"+std::to_string(pwInfo.timeRegistered)] = pInfo;
}
/**
* @brief deposite dip to the contract addr, the deposite value shouldn't be less than 0.001DIP
*/ 
PAYABLE void PasswordManage::depositeContract(){
    uint64_t value = dipc::callValueUDIP() * 1000;
    DIPC_EMIT_EVENT(depositingValue,value);
}
/**
* @brief set the unification expire time of the password
* @param _expireTime: the expire time of the password,the unit is hour
*/ 
EXPORT void PasswordManage::setExpireTime(uint64_t _expireTime){
    isOwner();
    // math.pow() function hasn't effort is vm, so ....
    *expireTime = _expireTime * 60 * 60 * 1000000000;
    DIPC_EMIT_EVENT(expireTimeSetted,expireTime.get());
}