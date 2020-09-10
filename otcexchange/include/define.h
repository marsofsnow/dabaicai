#pragma once


#define BALANCE_TYPE_AVAILABLE     1
#define BALANCE_TYPE_FREEZE        2


 #define        DEAL_STATUS_CREATED               1  //创建完成
 #define        DEAL_STATUS_PAYFIAT_PENING        2  //法币待支付完成
 #define        DEAL_STATUS_PAYFIAT_FINISH        3  //法币支付完成
 #define        DEAL_STATUS_PAYCOIN_PENDING       4  //放币中
 #define        DEAL_STATUS_PAYCOIN_FINISH        5  //放币完成
 #define        DEAL_STATUS_ARBITRATE_PENDING     6  //仲裁中
 #define        DEAL_STATUS_ARBITRATE_FINISH      7  //仲裁结束
 #define        DEAL_STATUS_FINISH                8  //deal over


 #define MARKET_STATUS_ON    1   //允许交易
 #define MARKET_STATUS_OFF   2   //不允许交易

#define  ORDER_STATUS_CREATED      1    //创建完成
#define  ORDER_STATUS_EXCHANGING   2    //交易中
#define  ORDER_STATUS_MANU_CANCEL  3    //手动取消
#define  ORDER_STATUS_FINISH       30   //结束


#define MARKET_ORDER_TYPE_LIMIT    1
#define MARKET_ORDER_TYPE_MARKET   2


#define MARKET_ORDER_SIDE_INVAID   0
#define MARKET_ORDER_SIDE_ASK      1
#define MARKET_ORDER_SIDE_BID      2

#define MARKET_ROLE_INVAID         0
#define MARKET_ROLE_MAKER          1
#define MARKET_ROLE_TAKER          2

#define MARKET_ORDER_SIDE_ASK_STR "ask"
#define MARKET_ORDER_SIDE_BID_STR "bid"
 
#define MARKET_ROLE_MAKER_STR      "mk" //maker的简称 12字符的限制啊
#define MARKET_ROLE_TAKER_STR      "tk" //taker的简称


#define ROLE_INVALID_STR    " role must be 1(广告挂单mk) or  2(OTC下单tk）"
#define SIDE_INVALID_STR    " side must be 1(ask)     or 2(bid）"
#define PAIR_EXIST_STR      " pair is exist"
#define PAIR_NOT_EXIST_STR      " is not exist"
#define DEAL_NOT_EXIST_STR      " deal  not exist"
#define ORDER_NOT_EXIST_STR      " order  not exist"
#define DEAL_STATUS_SAME        " deal status same"
#define MAKER_ORDER_NOR_EXIST_STR     " maker order not exist"

#define TOKEN_SYMBOL_CODE                "SYS"  //代币的符号

#define EOS_SYMBOL symbol(symbol_code("EOS") 4)

#define SG_MARKETS_SCOPE_STR       "markets"

#define ERR_CANCEL_ORDER_NOT_BELONG_TO     " not can cancel other user order"
#define ERR_RM_ORDER_NOT_BELONG_TO         " not can cancel other user order"
#define ERR_ORDER_CANCELED                 " order has be canceled"
#define ERR_ORDER_SIDE_SAME                " order side is same"





 