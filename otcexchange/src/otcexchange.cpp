#include <otcexchange.hpp>
#include <eosio/print.hpp>
#include <algorithm>


//#include "nlohmann/json.hpp"
//using namespace nlohmann;

ACTION otcexchange::hi(name nm)
{
   print_f("Name : %\n", nm);
   
   
}

ACTION otcexchange::putmkorder(const std::string &market,
                               const std::string &side,
                               name      user,
                               uint64_t  price,
                               uint64_t  amount,
                               uint64_t  min_amount,
                               uint64_t  max_amount,
                               const std::string &source)
{
   require_auth(user);
   check((side == MARKET_ORDER_SIDE_ASK_STR || side == MARKET_ORDER_SIDE_BID_STR), SIDE_INVALID_STR);
   



   //要注意name类型的长度限制
   std::string scope = market + MARKET_ROLE_MAKER_STR + side;
   transform(scope.begin(),scope.end(),scope.begin(),::tolower);
   

   
   order_index_t mk_orders(_self, name{scope}.value);

   mk_orders.emplace(user, [&](order &order) {
      order.id = mk_orders.available_primary_key();

      order.user           = user;                                        //订单所属用户，注意是name类型
      order.ctime          = current_time_point();                        //订单创建时间，精确到微秒
      order.utime          = order.ctime;                                 //订单更新时间，精确到微秒
      order.status         = ORDER_STATUS_CREATED;                        //订单状态，吃单和挂单的状态不同
      order.side           = side_to_uint(side);                          //买卖类型，1卖 2买
      order.type           = MARKET_ORDER_TYPE_LIMIT;                     //订单类型，1限价 2市价
      order.role           = MARKET_ROLE_MAKER;                           //订单类型，1挂单 2吃单
      order.price          = price;                                       //订单交易价格
      order.amount         = amount;                                      //订单交易数量
      order.min_amount     = min_amount;                                  //订单最小成交数量,默认是2
      order.max_amount     = max_amount;                                  //订单最小成交数量,默认是2
      order.taker_fee_rate = 1000;                                        //吃单的手续费率
      order.maker_fee_rate = 1000;                                        //挂单的手续费率
      order.left           = amount;                                      //剩余多少数量未成交
      order.freeze         = 0;                                           //冻结的stock或者money
      order.deal_fee       = 0;                                           //累计的交易手续费
      order.deal_stock     = 0;                                           //累计的交易sotck数量
      order.deal_money     = 0;                                           //累计的交易money
      order.source         = source;                                      //备注信息，订单来源
      
   });
}

ACTION otcexchange::puttkorder(const std::string &market,
                               const std::string &side,
                               name user,
                               uint64_t price,
                               uint64_t amount,
                               uint64_t mk_order_id,
                               const std::string &source)

{
   //cc push action otc puttkorder '["adxcny","bid","zhouhao",10,100,0,"我来买币"]' -p zhouhao
   require_auth(user);
   check((side == MARKET_ORDER_SIDE_ASK_STR || side == MARKET_ORDER_SIDE_BID_STR), SIDE_INVALID_STR);

   //1.找到对手方订单的消息
   std::string mk_side("");
   if(side==MARKET_ORDER_SIDE_ASK_STR){
      mk_side = MARKET_ORDER_SIDE_BID_STR;
   }else{
      mk_side = MARKET_ORDER_SIDE_ASK_STR;
   }

   

   std::string scope_maker = market + MARKET_ROLE_MAKER_STR + mk_side;
   transform(scope_maker.begin(),scope_maker.end(),scope_maker.begin(),::tolower);
  

   order_index_t mk_orders(_self, name{scope_maker}.value);
   auto mk_it = mk_orders.require_find(mk_order_id, MAKER_ORDER_NOR_EXIST_STR);
   const auto& mk_user= mk_it->user; 

   check(mk_it->side!=side_to_uint(side),ERR_ORDER_SIDE_SAME);





   std::string scope = market + MARKET_ROLE_TAKER_STR + side;
   transform(scope.begin(),scope.end(),scope.begin(),::tolower);
  
   order_index_t tk_orders(_self, name{scope}.value);

   auto tk_side = side_to_uint(side);
   uint64_t tk_order_id = tk_orders.available_primary_key();
   uint64_t deal_id = 0;


   tk_orders.emplace(user, [&](order &order) {
      order.id             = tk_order_id ;
      order.user           = user;                                        //订单所属用户，注意是name类型
      order.ctime          = current_time_point();                        //订单创建时间，精确到微秒
      order.utime          = order.ctime;                                 //订单更新时间，精确到微秒
      order.status         = ORDER_STATUS_EXCHANGING;                     //订单状态，吃单和挂单的状态不同
      order.side           = tk_side;                                     //买卖类型，1卖 2买
      order.type           = MARKET_ORDER_TYPE_LIMIT;                     //订单类型，1限价 2市价
      order.role           = MARKET_ROLE_TAKER;                    //订单类型，1挂单 2吃单
      order.price          = price;                                       //订单交易价格
      order.amount         = amount;                                      //订单交易数量
      order.min_amount     = 2;                                           //订单最小成交数量,默认是2
      order.taker_fee_rate = 1000;                                        //吃单的手续费率
      order.maker_fee_rate = 1000;                                        //挂单的手续费率
      order.left           = amount;                                      //剩余多少数量未成交
      order.freeze         = 0;                                           //冻结的stock或者money
      order.deal_fee       = 0;                                           //累计的交易手续费
      order.deal_stock     = 0;                                           //累计的交易sotck数量
      order.deal_money     = 0;                                           //累计的交易money
      order.source         = source;                                      //备注信息，订单来源

      std::string sc(market);
      transform(sc.begin(),sc.end(),sc.begin(),::toupper);


      deal_id = adddeal( user,
                           tk_side,
                           tk_order_id,
                           user,
                           mk_order_id,
                           mk_user,
                           price,
                           amount,
                           0,
                           0,
                           DEAL_STATUS_CREATED,
                           symbol_code{sc},
                           ""
                        );

       
       order.vec_deal.emplace_back(deal_id);

   });

   mk_orders.modify(mk_it,user,[&deal_id](order& o){
      o.vec_deal.emplace_back(deal_id);
      o.source.append("|").append("被吃单");
      o.utime  =current_time_point();
      o.status = ORDER_STATUS_EXCHANGING;
   });
     
}


ACTION otcexchange::newmarket(const std::string &stock,
                              const std::string &money,
                              uint8_t stock_prec,
                              uint8_t money_prec,
                              uint64_t min_amount)
{

   require_auth(get_self()); //必须要求是合约账号的权限

   market_index_t markets(get_self(), get_self().value); //table是合约范围内的

   auto pair = std::move(stock + money);

   auto it = markets.find(symbol_code(pair).raw());

   check(it == markets.end(), pair + PAIR_EXIST_STR);

   markets.emplace(_self, [&](market &m) {
      m.pair = symbol_code(pair);
      m.stock = stock;
      m.money = money;
      m.stock_prec = stock_prec;
      m.money_prec = money_prec;
      m.fee_prec = stock_prec;
      m.min_amount = min_amount;
      m.status =  MARKET_STATUS_ON;
      m.ctime = time_point_sec(current_time_point().sec_since_epoch());
      m.utime = time_point_sec(current_time_point().sec_since_epoch());
   });
}

ACTION otcexchange::closemarket(const std::string &stock,
                                const std::string &money)
{
   require_auth(_self);                                  //必须要求是合约账号的权限
   market_index_t markets(get_self(), get_self().value); //table是合约范围内的

   auto pair = std::move(stock + money);

   auto it = markets.find(symbol_code(pair).raw());

   check(it != markets.end(), pair + PAIR_NOT_EXIST_STR);
   if (it->status !=  MARKET_STATUS_OFF)
   {
      markets.modify(it, _self, [](market &m) { 
         m.status =  MARKET_STATUS_OFF; 
         m.utime = time_point_sec(current_time_point().sec_since_epoch());
      });
      
   }
}

ACTION otcexchange::openmarket(const std::string &stock,
                               const std::string &money)
{
   require_auth(_self);                                  //必须要求是合约账号的权限
   market_index_t markets(get_self(), get_self().value); //table是合约范围内的

   auto pair = std::move(stock + money);

   auto it = markets.find(symbol_code(pair).raw());

   check(it != markets.end(), pair + PAIR_NOT_EXIST_STR);
   if (it->status !=  MARKET_STATUS_ON)
   {
      markets.modify(it, _self, [](market &m) { 
         m.status =  MARKET_STATUS_ON; 
         m.utime = time_point_sec(current_time_point().sec_since_epoch());
      });
      
   }
}


ACTION otcexchange::rmmarkets()
{
  
   require_auth(_self);                                  //必须要求是合约账号的权限
   market_index_t markets(get_self(), get_self().value); //table是合约范围内的
   auto it = markets.begin();
   while (it != markets.end())
   {
      it = markets.erase(it);
   }
   print("delete all market finish\n");
}

ACTION otcexchange::rmmarket(const std::string &stock,
                             const std::string &money)
{
   require_auth(_self);                                  //必须要求是合约账号的权限
   market_index_t markets(get_self(), get_self().value); //table是合约范围内的

   auto pair = std::move(stock + money);

   auto it = markets.find(symbol_code(pair).raw());

   check(it != markets.end(), pair + PAIR_NOT_EXIST_STR);
   markets.erase(it);
   print("delete one market finish\n");
}

/*
币币交易
资产解冻过程
balance=balance+amount
freeze=freeze-amount

资产冻结过程
freeze=freeze+amount
balance=balance-amount

订单第一次挂在盘口
买stock：money资产冻结freeze=order.price*order.left，更新订单的freeze字段
卖stock：stock资产冻结freeze=order.left，更新订单的freeze字段

订单撤销结束时
买stock：money资产解冻freeze
卖stock：stock资产解冻freeze


撮合过程中要计算（主动方式卖）
price 
amount
deal    = price*amount        // 代表交易额
ask_fee = deal*taker_fee     //  卖币方手续费
bid_fee = amount* maker_fee  //  买方手续费

//1.1-------------taker(假如是卖，主动发起）---------------
left=left-amount
deal_stock=deal_stock+amount //  已经交易的代币数
deal_money=deal_money+deal  //   已经交易的miney数目
deal_fee = deal_fee+ask_fee 

stock：type_availavle减amount
money：type_available加deal减ask_fee

如果taker要放在盘口,走冻结流程


//1.1------------maker(假如是买)----------------
left = left-amount;
freeze = freeze-deal; //这么多的钱不需要冻结，更新冻结字段
deal_stock=deal_stock+amount;
deal_money=deal_money+deal
deal_fee = deal_fee+bid_fee

money：type_freeze减去deal
stock：type_available加amount 2.减bid_fee






撮合过程中要计算（主动方式买）
price 
amount
deal    = price*amount        // 代表交易额
ask_fee = deal*(maker->maker_fee）    //  卖币方手续费
bid_fee = amount* （taker->taker_fee）  //  买方手续费

//1.1-------------taker(假如是买，主动发起）---------------
left=left-amount
deal_stock=deal_stock+amount //  已经交易的代币数
deal_money=deal_money+deal  //   已经交易的miney数目
deal_fee = deal_fee+bid_fee 

stock：type_availavle加amount减bid_fee
money：type_availavle减deal

如果taker要放在盘口,走冻结流程


//1.1------------maker(假如是卖币)----------------
left = left-amount;
freeze = freeze-amount; //这么多的代币不需要冻结，更新冻结字段
deal_stock=deal_stock+amount;
deal_money=deal_money+deal
deal_fee = deal_fee+ask_fee

money：type_available加deal减ask_fee
stock：type_freeze减amount




*/