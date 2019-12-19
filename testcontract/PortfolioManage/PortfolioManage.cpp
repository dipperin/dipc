#include "PortfolioManage.hpp"

    /**
     * @brief init contract params
     * //@param depositValue: the value of owner transfer to the contract，the unit is WU 
     * @return void
     */
    PAYABLE void PortfolioManage::init(){
        *owner = caller().toString();
        *changeRate = 10000;
        *userCanWithdraw = false;
        uint64_t tokenBalance = getTotalBalance();
        //DipcAssert(depositValue == callValue());
        *ownerTotalToken = tokenBalance;
        *totalToken = ownerTotalToken.get();
        //print("tokenBalance");
        //print(tokenBalance);
        DIPC_EMIT_EVENT(initEvent, "ownerTotalToken" ,tokenBalance);
    }
    
     /**
     * @brief set the change rate between DIP and current token
     * @param _changeRate: the change rate between DIP and current token
     * @return void
     */
    EXPORT void PortfolioManage::setChangeRate(uint64_t _changeRate){
        isOwner();
        *changeRate = _changeRate;
    }
     /**
     * @brief set whether the user can withdraw token by himself 
     * @param _canWithdraw: true or false -- whether the user can withdraw token by himself 
     * @return void
     */
    EXPORT void PortfolioManage::setUserCanWithdraw(bool _canWithdraw){
        isOwner();
        *userCanWithdraw = _canWithdraw;
    }

    /**
     * @brief create a portfolio
     * @param _portfolioName: the name of current portfolio
     * @param _portfolioDesc: the description of current portfolio
     * @return void
     */
    PAYABLE void PortfolioManage::createPortfolio(char* _portfolioName, char* _portfolioDesc){
        if(portfolioStorge.get()[_portfolioName] != ""){
            DIPC_EMIT_EVENT(ErrEvent, "Current portfolio exist: ", _portfolioName);
            return;
        } 
        std::string callerAddr = caller().toString();
        uint64_t tokenBalance = getTotalBalance();
        // changeU256ToUint64WithChangeRate(callValue(), changeRate.get());
        PortfolioInfo info(_portfolioName, _portfolioDesc, &callerAddr[0]);
        std::string portfolioStr = info.toJson();
        //print("createPortfolio");
        //print(portfolioStr);
        //print(info.getPortfolioResult());

        (*portfolioStorge)[_portfolioName] = portfolioStr;
        DipcAssert(totalToken.get() + tokenBalance > totalToken);
        (*portfolioBalance)[_portfolioName] = tokenBalance;
        *totalToken = totalToken.get() + tokenBalance; 
        //u256 callV = callValue();
        DIPC_EMIT_EVENT(portfolioInfo, _portfolioName, &portfolioStr[0], "portfolioBalance", tokenBalance);
    }

    /**
     * @brief deposit a portfolio
     * @param _portfolioName: the name of the portfolio  want to deposit
     * @return void
     */
    PAYABLE void PortfolioManage::depositPortfolio(char* _portfolioName){
        uint64_t tokenBalance = getTotalBalance();
        //uint64_t tokenBalance = changeU256ToUint64WithChangeRate(callValue(), changeRate.get());
        uint64_t currentBalance = portfolioBalance.get()[_portfolioName];
        uint64_t finalBalance  = currentBalance + tokenBalance;
        DipcAssert(totalToken.get() + tokenBalance > totalToken);
        DipcAssert(finalBalance > currentBalance);
        (*portfolioBalance)[_portfolioName] = finalBalance;
        *totalToken = totalToken.get() + finalBalance; 
        DIPC_EMIT_EVENT(depositPortfolio, "portfolioName", _portfolioName, "portfolioBalance", finalBalance);
    }
    /**
     * @brief create a order
     * @param _portfolioName: the name of the portfolio  current order blong to   
     * @param _orderId: orderId used by application
     * @param _orderType
     * @param _stockCode
     * @param _stockPrice: unit is token
     * @param _stockAmount: uint is one share 
     * @param _endTime: nanoseconds the order expire
     * @return void
     */
    PAYABLE void PortfolioManage::createOrder(char* _portfolioName, char* _orderId, uint16_t _orderType, char* _stockCode, uint64_t _stockPrice, uint64_t _stockAmount, uint64_t _endTime){
        uint64_t tokenBalance = getTotalBalance();
        DIPC_EMIT_EVENT(createOrder, "portfolioName", _portfolioName, "OrderInfo", "ignore", "tokenBalance", tokenBalance);
        uint64_t spent = 0;
        if(_orderType == 1){
            spent = _stockPrice * _stockAmount;
            DipcAssert((portfolioBalance.get()[_portfolioName] + tokenBalance) > spent);
        }
        struct Order order{_orderId, _orderType, _stockCode, _stockPrice, _stockAmount, _endTime, 0};
        std::string pInfo = portfolioStorge.get()[_portfolioName];
        //print("identity start");
        //print(pInfo);
        //print(tokenBalance);
        if(pInfo == "") {
            DIPC_EMIT_EVENT(ErrEvent, "Current portfolio  doesn't exist: ", _portfolioName);
            return;
        }
        PortfolioInfo info;
        info.parseJson(pInfo);
        if(info.getOrder(_orderId).orderId != ""){
            DIPC_EMIT_EVENT(ErrEvent, "Current order exist: ", _orderId);
            return;
        }
        info.addOrder(order);
        pInfo = info.toJson();

        uint64_t currentBalance = portfolioBalance.get()[_portfolioName] ;
        uint64_t finalBalance = currentBalance + tokenBalance - spent;
        DipcAssert(totalToken.get() + tokenBalance > totalToken);
        //DipcAssert(finalBalance > currentBalance);
        DipcAssert(ownerTotalToken.get() < ownerTotalToken.get() + spent);
        (*portfolioStorge)[_portfolioName] = &pInfo[0];
        //print("identity ten");
        //print(pInfo);
        //print(finalBalance);
        //print(spent);


        (*portfolioBalance)[_portfolioName] = finalBalance;
        *totalToken = totalToken.get() + finalBalance; 
        *ownerTotalToken = ownerTotalToken.get() + spent;
        DIPC_EMIT_EVENT(createOrder, "portfolioName", _portfolioName, "OrderInfo", &order.toJson()[0], "portfolioBalance", finalBalance);
    }

    /**
     * @brief 
     * 
     * 
     * 
     */
    EXPORT void PortfolioManage::dealOrder(char* _portfolioName, char* _orderId){
        isOwner();
        std::string pInfo = portfolioStorge.get()[_portfolioName];
        //print("dealOrder  pInfo");
        //print(pInfo);
        PortfolioInfo info;
        info.parseJson(pInfo);
        //print("dealOrder  start");

        //print(info.toJson());
         Order ord = info.getOrder(_orderId);
         //print("dealOrder");
         //print(ord.toJson());
        if(ord.orderId != "" && ord.orderStatus == 0){
            //DipcAssert(info.getOwnerAddress() == caller().toString());
            uint64_t timenow = dipc::timestamp();
            if(ord.endTime > timenow){
                ord.orderStatus = 1;
            }else {
                ord.orderStatus = 3;
            }
        } else {
          // print("dealOrder  return ");

            return;
        }
        // print("dealOrder  one ");

        uint64_t sellValue = 0;
        if(ord.orderType == 0){
            sellValue = ord.stockAmount * ord.stockPrice;
        }
        uint64_t currentBalance =  portfolioBalance.get()[_portfolioName];
        // print("dealOrder  two");

        DipcAssert(currentBalance + sellValue >= currentBalance);
        DipcAssert((ownerTotalToken.get() - sellValue) <= ownerTotalToken.get());
        


        (*portfolioBalance)[_portfolioName] = currentBalance + sellValue;
        *ownerTotalToken = ownerTotalToken.get() - sellValue;
        (*portfolioStorge)[_portfolioName] = &info.toJson()[0];
        //print("dealOrder  three");
        //print(info.toJson());
        //print(currentBalance + sellValue);
        //print(ownerTotalToken.get());

        DIPC_EMIT_EVENT(dealPortfolio, "orderId", _orderId, "orderInfo", &ord.toJson()[0]);
    }
    /**
     * @brief 
     * 
     * 
     * 
     */
    EXPORT void PortfolioManage::revocationOrder(char* _portfolioName, char* _orderId){
        std::string pInfo = portfolioStorge.get()[_portfolioName];
        //print("pInfo");
        //print(pInfo);
        PortfolioInfo info;
        info.parseJson(pInfo);
        Order ord = info.getOrder(_orderId);
        //print("ordInfo");
        //print(ord.toJson());
        if(ord.orderId != "" && ord.orderStatus == 0){
            std::string callerStr = caller().toString();
            //print("revocationOrder start");
            //print(info.getOwnerAddress());
            //print(callerStr);
            DipcAssert(info.getOwnerAddress() == caller().toString());
            //print("revocationOrder end");

            uint64_t timenow = dipc::timestamp();
            if(ord.endTime > timenow){
                ord.orderStatus = 2;
            }else {
                ord.orderStatus = 3;
            }
        } else {
            return;
        }
        uint64_t lockToken = 0;
        if(ord.orderType == 1){
            lockToken = ord.stockPrice * ord.stockAmount;
        }
        uint64_t currentBalance =  portfolioBalance.get()[_portfolioName];
        //print("revocationOrder can see");
        //print(currentBalance);
        // print(ownerTotalToken.get());
        // print(lockToken);

        DipcAssert(currentBalance + lockToken >= currentBalance);
        DipcAssert(ownerTotalToken.get() - lockToken <= ownerTotalToken.get());

        //print("revocationOrder can see  two");
        (*portfolioBalance)[_portfolioName] = currentBalance + lockToken;
        *ownerTotalToken = ownerTotalToken.get() - lockToken;
        (*portfolioStorge)[_portfolioName] = &info.toJson()[0];
        DIPC_EMIT_EVENT(revocationPortfolio, "orderId", _orderId, "orderInfo", &ord.toJson()[0]);
    }
    /**
     * @brief 
     * @param _withdrawBalance:  the unit is token
     * 
     * 
     */
    EXPORT void PortfolioManage::withdrawPortfolio(char* _portfolioName, uint64_t _withdrawBalance){
         if(!userCanWithdraw.get()){
             return;
         }
         uint64_t currentBalance =  portfolioBalance.get()[_portfolioName];
         DipcAssert(currentBalance - _withdrawBalance < currentBalance);
         auto coinBalance = changeUint64ToU256WithChangeRate(currentBalance, changeRate.get());
         (*portfolioBalance)[_portfolioName] = currentBalance - _withdrawBalance;
         callTransfer(caller(), coinBalance);
    }
    /**
     * @brief owner deposit to the contract address to get more token
     * @param _depositValue: the value num transfer to the contract address
     */
    EXPORT void PortfolioManage::withdrawPool(uint64_t _balance){
        isOwner();
        Address ownerAddr = Address(owner.get()); 
        callTransfer(ownerAddr, _balance);
        *ownerTotalToken = ownerTotalToken.get() - _balance * changeRate.get();
        *totalToken = totalToken.get() - _balance * changeRate.get();
        DIPC_EMIT_EVENT(tokenCount, "ownerTotalToken", ownerTotalToken.get(), "totalToken", totalToken.get());
    }
    /**
     * @brief owner deposit to the contract address to get more token
     * @param _depositValue: the value num transfer to the contract address
     */
    PAYABLE void PortfolioManage::depositPool(uint64_t _depositValue){
        isOwner();
        //DipcAssert(_depositValue == dipc::callValue());
        uint64_t tokenBalance = changeU256ToUint64WithChangeRate(callValue(), changeRate.get()); 
        *ownerTotalToken = ownerTotalToken.get() + tokenBalance;
        *totalToken = totalToken.get() + tokenBalance;
        DIPC_EMIT_EVENT(tokenCount, "ownerTotalToken", ownerTotalToken.get(), "totalToken", totalToken.get());
    }
     /**
     * @brief query the total token in the contract
     * @return the total token int the contract
     */
    CONSTANT uint64_t PortfolioManage::queryTotalToken(){
        return totalToken.get();
    }

     /**
     * @brief query the token balance of the portfolio
     * @param _portfolioName : the portfolio name
     * @return the balance of the portfolio, the unit is token
     */
    CONSTANT uint64_t PortfolioManage::queryPortfolioBalance(char* _portfolioName){
        return portfolioBalance.get()[_portfolioName];
    }

    /**
     * @brief query the portfolio name  whether  used or not 
     * @return true is used, false is not used
     */
    CONSTANT bool PortfolioManage::queryPortfolioNameIfUsed(char* _portfolioName){
        if(portfolioStorge.get()[_portfolioName] != ""){
            return true;
        } 
        return false;
    }

    /**
     * @brief query the change rate between DIP and current token
     * @return the change rate between DIP and current token
     */
    CONSTANT uint64_t PortfolioManage::queryChangeRate(){
        return changeRate.get();
    }