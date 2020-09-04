#pragma once


#define BALANCE_TYPE_AVAILABLE     1
#define BALANCE_TYPE_FREEZE        2

#define MARKET_ORDER_TYPE_LIMIT    1
#define MARKET_ORDER_TYPE_MARKET   2

#define MARKET_ORDER_SIDE_ASK      1
#define MARKET_ORDER_SIDE_BID      2

#define MARKET_ROLE_MAKER          1
#define MARKET_ROLE_TAKER          2

#define MARKET_ORDER_SIDE_ASK_STR "ask"
#define MARKET_ORDER_SIDE_BID_STR "bid"
 
#define MARKET_ROLE_MAKER_STR      "mk" //maker的简称 12字符的限制啊
#define MARKET_ROLE_TAKER_STR      "tk" //taker的简称


#define ROLE_INVALID_STR    "role must be 1(广告挂单) or  2(OTC下单）"
#define SIDE_INVALID_STR    "side must be 1(ask)     or 2(bid）"

#define TOKEN_SYMBOL_CODE                "SYS"  //代币的符号