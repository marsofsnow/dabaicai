#pragma once

#define BALANCE_TYPE_AVAILABLE 1
#define BALANCE_TYPE_FREEZE 2

#define DEAL_STATUS_UNPAID 10                  //未付款
#define DEAL_STATUS_UNPAID_MAN_CANCELED 11     //未付款手动取消
#define DEAL_STATUS_UNPAID_TIMEOUT_CANCELED 12 //未付款超时取消

#define DEAL_STATUS_PAID 20              //已付款
#define DEAL_STATUS_PAID_UNPLAYCOIN 21   //已付款待放币(买卖方都可以申述)
#define DEAL_STATUS_PAID_ASK_APPEAL 22   //已付款待放币(买卖方都可以申述)
#define DEAL_STATUS_PAID_BID_APPEAL 23   //已付款待放币(买卖方都可以申述)
#define DEAL_STATUS_PAID_ARBIARATE 24    //已付款待放币仲裁中
#define DEAL_STATUS_PAID_PLAYCOIN_ING 25 //放币中
#define DEAL_STATUS_PAID_PLAYCOIN_ED 25  //放币完成

#define MARKET_STATUS_ON 1  //允许交易
#define MARKET_STATUS_OFF 2 //不允许交易

#define MARKET_STATUS_ON_STR "allow trade"      //允许交易
#define MARKET_STATUS_OFF_STR "not allow trade" //不允许交易

#define AD_STATUS_ONTHESHELF 1      //上架中
#define AD_STATUS_MAN_OFFTHESHELF 2 //手动下架
#define AD_STATUS_AUT_OFFTHESHELF 3 //自动下架
#define AD_STATUS_CANCELED 4        //已撤销
#define AD_STATUS_FINISHED 5        //已完成

#define MARKET_ORDER_TYPE_LIMIT 1
#define MARKET_ORDER_TYPE_MARKET 2

#define MARKET_ORDER_SIDE_INVAID 0
#define MARKET_ORDER_SIDE_ASK 1
#define MARKET_ORDER_SIDE_BID 2

#define MARKET_ROLE_INVAID 0
#define MARKET_ROLE_MAKER 1
#define MARKET_ROLE_TAKER 2

#define MARKET_ORDER_SIDE_ASK_STR "ask"
#define MARKET_ORDER_SIDE_BID_STR "bid"

#define MARKET_ROLE_MAKER_STR "mk" //maker的简称 12字符的限制啊
#define MARKET_ROLE_TAKER_STR "tk" //taker的简称

#define ROLE_INVALID_STR " role must be 1(广告挂单mk) or  2(OTC下单tk）"
#define SIDE_INVALID_STR " side must be 1(ask)     or 2(bid）"
#define PAIR_EXIST_STR " pair is exist"
#define PAIR_NOT_EXIST_STR " is not exist"
#define DEAL_NOT_EXIST_STR " deal  not exist"
#define ORDER_NOT_EXIST_STR " order  not exist"
#define DEAL_STATUS_SAME " deal status same"
#define MAKER_ORDER_NOR_EXIST_STR " maker order not exist"

#define TOKEN_SYMBOL_CODE "SYS" //代币的符号

#define EOS_SYMBOL symbol(symbol_code("EOS") 4)

#define SG_MARKETS_SCOPE_STR "markets"

#define ERR_CANCEL_ORDER_NOT_BELONG_TO " not can cancel other user order"
#define ERR_RM_ORDER_NOT_BELONG_TO " not can cancel other user order"
#define ERR_ORDER_CANCELED " order has be canceled"
#define ERR_ORDER_SIDE_SAME " order side is same"
#define ERR_FORBID_SELF_EXCHANGE " forbid self exchange"
#define ERR_NOT_ENOUGH_TOKEN_TO_SELL " counterparty not enough token to sell"
#define ERR_CHECK_PRICE_GREAT_ZERO " price  must > 0"
#define ERR_CHECK_AMOUNT_GREAT_ZERO " amount must > 0"
#define ERR_CHECK_FORBIN_CANCEL_SELL_ORDER " forbid cancel ask order"
