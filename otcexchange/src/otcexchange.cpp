#include <otcexchange.hpp>

const name otcexchange::setting_scope="setting"_n;

ACTION otcexchange::hi( name nm ) {
   /* fill in action body */
   print_f("Name : %\n",nm);
}

ACTION otcexchange::putmkorder(const std::string& market,
                               const std::string& side,
                               name               user,
                               uint64_t           price,
                               uint64_t           amount,
                               const std::string& source
                  )
{
   require_auth(user);
   check((side==MARKET_ORDER_SIDE_ASK_STR || side==MARKET_ORDER_SIDE_BID_STR), SIDE_INVALID_STR);
   
   //要注意name类型的长度限制
   std::string scope=market+MARKET_ROLE_MAKER_STR+side;
   order_index_t mk_orders(_self,name{scope}.value);

   mk_orders.emplace(user,[&](order& order){
      order.order_id = mk_orders.available_primary_key();

      order.user   = user;                         //订单所属用户，注意是name类型
      order.ctime  = current_time_point();         //订单创建时间，精确到微秒
      order.utime  = order.ctime;                  //订单更新时间，精确到微秒
      order.status = 1;                            //订单状态，吃单和挂单的状态不同
      order.side   = side_to_uint(side);           //买卖类型，1卖 2买
      order.type   = MARKET_ORDER_TYPE_LIMIT;      //订单类型，1限价 2市价
      order.role   = MARKET_ROLE_MAKER;            //订单类型，1挂单 2吃单
      order.price  = price;                        //订单交易价格
      order.amount = amount;                       //订单交易数量
      order.min_amount = 2;                        //订单最小成交数量
      order.taker_fee_rate = 1000;                 //吃单的手续费率
      order.maker_fee_rate = 1000;                 //挂单的手续费率
      order.left           = amount;               //剩余多少数量未成交
      order.freeze         = 0;                                //冻结的stock或者money
      order.deal_fee       = 0;                                //累计的交易手续费
      order.deal_stock     = 0;                                //累计的交易sotck数量
      order.deal_money     = 0;                                //累计的交易money
      order.source         = source;                           //备注信息，订单来源
      //order.vec_deal=std::vector<deal>{};                    //成交明细
      

   });
}
      
ACTION otcexchange::puttkorder(  const std::string&  market,
                                 const std::string&  side,
                                 name                user,
                                 uint64_t            price,
                                 uint64_t            amount,
                                 uint64_t            deal_order_id,
                                 const std::string&  source
                  )

{
   require_auth(user);

   check((side==MARKET_ORDER_SIDE_ASK_STR || side==MARKET_ORDER_SIDE_BID_STR), SIDE_INVALID_STR);
   

   std::string scope=market+MARKET_ROLE_MAKER_STR+side;
   order_index_t tk_orders(_self,name{scope}.value);

   tk_orders.emplace(user,[&](order& order){
      order.order_id = tk_orders.available_primary_key();

      order.user   = user;                         //订单所属用户，注意是name类型
      order.ctime  = current_time_point();         //订单创建时间，精确到微秒
      order.utime  = order.ctime;                  //订单更新时间，精确到微秒
      order.status = 1;                            //订单状态，吃单和挂单的状态不同
      order.side   = side_to_uint(side);                         //买卖类型，1卖 2买
      order.type   = MARKET_ORDER_TYPE_LIMIT;      //订单类型，1限价 2市价
      order.role   = MARKET_ROLE_TAKER;            //订单类型，1挂单 2吃单
      order.price  = price;                        //订单交易价格
      order.amount = amount;                       //订单交易数量
      order.min_amount = 2;                        //订单最小成交数量
      order.taker_fee_rate = 1000;                 //吃单的手续费率
      order.maker_fee_rate = 1000;                 //挂单的手续费率
      order.left           = amount;               //剩余多少数量未成交
      order.freeze         = 0;                                //冻结的stock或者money
      order.deal_fee       = 0;                                //累计的交易手续费
      order.deal_stock     = 0;                                //累计的交易sotck数量
      order.deal_money     = 0;                                //累计的交易money
      order.source         = source;                           //备注信息，订单来源
      
      order.vec_deal.emplace_back(
                side_to_uint(side),
                deal_order_id,
                "zhouhao"_n,
                price,
                amount,
                0,
                0,
                0,
                0,
                source
      );                   
      

   });
}



ACTION otcexchange::orderbook(const std::string& market,const std::string& role,const std::string& side)
{
   check( (role==MARKET_ROLE_TAKER_STR      || role==MARKET_ROLE_MAKER_STR),      ROLE_INVALID_STR);
   check((side==MARKET_ORDER_SIDE_ASK_STR   || side==MARKET_ORDER_SIDE_BID_STR), SIDE_INVALID_STR);

    //要注意name类型的长度限制
   std::string scope=market+MARKET_ROLE_MAKER_STR+side;
   order_index_t orders(_self,name{scope}.value);
   auto price_index = orders.get_index<"byprice"_n>();
   auto itr = price_index.begin();
   while (itr!=price_index.end())
   {
      ++itr;
   }
   
}


ACTION otcexchange::createspot(const std::string& stock,
                           const std::string& money,
                           uint8_t stock_prec, 
                           uint8_t money_prec, 
                           uint64_t min_amount
                        )
{

   //print("1");
   
   init_otc();
   
   auto state = singleton_otc.get();
   
   market one(stock,money,stock_prec,money_prec,min_amount);
   print(one.stock);
   state.map_market.emplace(stock+money,one);
   state.set_title.emplace(stock+money);
   
   print(state.set_title.begin()->c_str());
   singleton_otc.set(state,_self);
   print("3");

}


ACTION otcexchange::banspot(const std::string& stock,
                               const std::string& money)
{
   init_otc();
   auto state = singleton_otc.get();
   std::string title(stock+money);
   state.map_market.erase(title);
   state.set_title.erase(title);
   singleton_otc.set(state,_self);
}


ACTION otcexchange::getspots(){
   if(singleton_otc.exists()){
      print(singleton_otc.get().set_title.begin()->c_str(),"\n");
   }else{
      print("singleton is empty!,why???\n");
   }
}



ACTION otcexchange::removespots(){
   if(singleton_otc.exists()){
   
      singleton_otc.remove();
   }else{
      print("singleton is empty!,why???\n");
   }
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