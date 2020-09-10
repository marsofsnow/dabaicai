#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/action.hpp>
#include <eosio/transaction.hpp>
#include <eosio/singleton.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include "common/define.hpp"
#include "define.h"



using namespace eosio;




CONTRACT otcexchange : public contract {
   public:
      using contract::contract;
      otcexchange(name self, name first_receiver, datastream<const char*> ds):
                  contract(self,first_receiver,ds){}

                                                                           

      ACTION hi( name nm );

      //using hi_action = action_wrapper<"hi"_n, &otcexchange::hi>;
      USING_ACTION(otcexchange,hi);



      


      
//------------------------define markets begin-------------------------------//
   private:
      

      //交易对 ADXCNY
      TABLE market{
         symbol_code         pair;      //交易对
         std::string         stock =""; //coin
         std::string         money =""; //fiat
         uint8_t             stock_prec = 4 ; 
         uint8_t             money_prec = 2 ;
         uint8_t             fee_prec   = 4 ;
         uint64_t            min_amount = 4 ;
         uint8_t             status     = MARKET_STATUS_ON;
         time_point_sec      ctime{current_time_point().sec_since_epoch()}; 
         time_point_sec      utime{current_time_point().sec_since_epoch()}; 


         uint64_t primary_key()            const { return pair.raw(); }
         uint64_t get_secondary_status()   const { return status; }   // sort by user name，按用户名过滤


         /*
         market(const std::string& stock="",
                const std::string& money="",
                uint8_t  stock_prec=4, 
                uint8_t  money_prec=2, 
                uint64_t min_amount=4):
                                     pair(stock+money),
                                     stock(stock),
                                     money(money),
                                     stock_prec(stock_prec),
                                     money_prec(money_prec),
                                     fee_prec(stock_prec),
                                     min_amount(min_amount),
                                     status(MARKET_STATUS::MARKET_STATUS_ON),
                                     ctime{current_time_point().sec_since_epoch()},
                                     utime{current_time_point().sec_since_epoch()}
                                     {}
         */

         EOSLIB_SERIALIZE( market, (pair)(stock)(money)(stock_prec)(money_prec)(fee_prec)(min_amount)(status)(ctime)(utime) )

               
      };

      using market_index_t = multi_index<"markets"_n,
                                       market,
                                       indexed_by<"bystatus"_n, const_mem_fun< market,uint64_t,&market::get_secondary_status> >
                                       > ;
      

      

   public:

       //new one  market 操作
      ACTION newmarket(const std::string& stock,
                           const std::string& money,
                           uint8_t  stock_prec, 
                           uint8_t  money_prec, 
                           uint64_t min_amount
                            );

      //交易对禁止交易
      ACTION closemarket(const std::string& stock,
                         const std::string& money);

      
      //交易对放开交易
      ACTION openmarket(const std::string& stock,
                        const std::string& money);




      //清空所有的交易对
      ACTION rmmarkets(); 
      ACTION rmmarket(  const std::string& stock,
                        const std::string& money); 


      USING_ACTION(otcexchange,newmarket);
      USING_ACTION(otcexchange,closemarket);
      USING_ACTION(otcexchange,openmarket);
      USING_ACTION(otcexchange,rmmarkets);
      USING_ACTION(otcexchange,rmmarket);


//------------------------define markets end-------------------------------//
//------------------------define deal  begin-------------------------------//


   private:

      TABLE balance{
         name    user;
         uint8_t type;
         asset   quantity;

      };

      TABLE assetlog{
         uint64_t       id;
         time_point_sec ctime;
         name           user;
         std::string    asset_name;
         std::string    business;
         int64_t        change;
         uint64_t       balance;
         std::string    detail;
      };



      TABLE deal{
         uint64_t        id;                    //deal id 合约全局唯一，自增id
         time_point      ctime;                 //创建时间
         time_point      utime;                 //更新时间
         uint8_t         taker_side;            //taker是买还是卖
         uint64_t        taker_order_id;         
         name            taker_user;             //对手方     
         uint64_t        maker_order_id;         //对手方订单id
         name            maker_user;             //对手方
         uint64_t        price;                  //成交价
         uint64_t        amount;                 //成交数量
         //uint64_t        deal_quota;           //成交额price*amount
         uint64_t        ask_fee;                //收取的卖方手续费
         uint64_t        bid_fee;                //收取的买方手续费
         uint8_t         status;                 //成交的状态
         symbol_code     pair;                   //交易对
         std::string     msg;                    //备注信息，仲裁信息等

         uint64_t primary_key() const { return id;}
         uint64_t get_secondary_pair() const { return pair.raw();}


      };

      using deal_index_t = multi_index<"deals"_n,
                                        deal,
                                       indexed_by<"bypair"_n,const_mem_fun< deal,uint64_t,&deal::get_secondary_pair > >
                                    
                                       > ;


   public:
      ACTION moddealstaus(uint64_t id,uint8_t status,const std::string& reason){
         deal_index_t deals(get_self(),get_self().value);
         auto it = deals.require_find(id,DEAL_NOT_EXIST_STR);
         if(it->status!=status){
            deals.modify(it,_self,[&](deal& d){
               d.status = status;
               d.msg.append("|").append(reason);
            });
         }
      }
      ACTION rmdeals(){
         require_auth(get_self());
         deal_index_t deals(get_self(),get_self().value);
         auto it = deals.begin();
         while (it!=deals.end())
         {
            it = deals.erase(it);
         }
         
      }

      USING_ACTION(otcexchange,moddealstaus);
      USING_ACTION(otcexchange,rmdeals);

   private:
      //返回值是deal id
      uint64_t adddeal(name payer,
                   uint8_t    taker_side,
                   uint64_t   taker_order_id,
                   name       taker_user,
                   uint64_t   maker_order_id,
                   name       maker_user,
                   uint64_t   price,
                   uint64_t   amount,
                   uint64_t   ask_fee,
                   uint64_t   bid_fee,
                   uint8_t    status,
                   symbol_code pair,
                   const std::string& msg
                  )
      {
         //自增id，全局唯一
         uint64_t res;
         deal_index_t deals(get_self(),get_self().value);
         deals.emplace(payer,[&](deal& d){
            d.id = deals.available_primary_key();
            d.ctime = current_time_point();
            d.utime = current_time_point();
            d.taker_side = taker_side;
            d.taker_order_id=taker_order_id;
            d.maker_order_id=maker_order_id;
            d.taker_user=taker_user;
            d.maker_user=maker_user;
            d.price=price;
            d.amount=amount;
            d.ask_fee = ask_fee;
            d.bid_fee=bid_fee;
            d.status=DEAL_STATUS_CREATED;
            d.pair=pair;
            d.msg=msg;
            res=d.id;
         });

         return res;

      }



//------------------------define deal  end-------------------------------//





   private:




      TABLE order{
         uint64_t      id;                          //订单id，自增主键,scope = market+side.相当于盘口
         name          user;                        //订单所属用户，注意是name类型
         time_point    ctime;                       //订单创建时间，精确到微秒
         time_point    utime;                       //订单更新时间，精确到微秒
         uint8_t       status;                      //订单状态，吃单和挂单的状态不同
         uint8_t       side;                        //买卖类型，1卖 2买
         uint8_t       type;                        //订单类型，1限价 2市价
         uint8_t       role;                        //订单类型，1挂单 2吃单
         uint64_t      price;                       //订单交易价格
         uint64_t      amount;                      //订单交易数量
         uint64_t      min_amount;                  //订单最小成交数量
         uint64_t      max_amount;                  //订单最大成交数量
         uint64_t      taker_fee_rate;              //吃单的手续费率
         uint64_t      maker_fee_rate;              //挂单的手续费率
         uint64_t      left;                        //剩余多少数量未成交
         uint64_t      freeze;                      //冻结的stock或者money,处于正在交易中
         uint64_t      deal_fee;                    //累计的交易手续费
         uint64_t      deal_stock;                  //累计的交易sotck数量
         uint64_t      deal_money;                  //累计的交易money
         std::string   source;                      //备注信息，订单来源
         std::vector<uint64_t> vec_deal;            //成交明细


         uint64_t primary_key()          const { return id; }          // primary key auto increase
         uint64_t get_secondary_user()   const { return user.value;}   // sort by user name，按用户名过滤
         uint64_t get_secondary_price()  const { return price;}        // sort by price, ask increase bid of deincrease
         
      };
      using order_index_t = multi_index<"orders"_n,
                                       order,
                                       indexed_by<"byuser"_n, const_mem_fun< order,uint64_t,&order::get_secondary_user >  >,
                                       indexed_by<"byprice"_n,const_mem_fun< order,uint64_t,&order::get_secondary_price > >
      > ;


   public:
      inline static uint8_t side_to_uint(const std::string& side){
         if(side=="ask") return MARKET_ORDER_SIDE_ASK;
         if(side=="bid") return MARKET_ORDER_SIDE_BID;
         return 0;
      }

      inline static uint8_t role_to_uint(const std::string& role){
         if(role=="mk") return MARKET_ROLE_MAKER;
         if(role=="tk") return MARKET_ROLE_TAKER;
         return MARKET_ROLE_INVAID;
      }


      

         //order操作
      ACTION putmkorder(const std::string& market,
                        const std::string& side,
                        name               user,
                        uint64_t           price,
                        uint64_t           amount,
                        uint64_t           min_amount,
                        uint64_t           max_amount,
                        const std::string& source
                        );
                        
      ACTION puttkorder(const std::string& market,
                        const std::string& side,
                        name               user,
                        uint64_t           price,
                        uint64_t           amount,
                        uint64_t           mk_order_id,
                        const std::string& source
                        );


      ACTION rmorders(const std::string& market,
                      const std::string& side,
                      const std::string& role
                      ){

         require_auth(get_self());
         check((side == MARKET_ORDER_SIDE_ASK_STR || side == MARKET_ORDER_SIDE_BID_STR), SIDE_INVALID_STR);
         check((role == MARKET_ROLE_MAKER_STR     || role == MARKET_ROLE_TAKER_STR),     ROLE_INVALID_STR);

         //要注意name类型的长度限制
         std::string scope = market + role + side;
         
         order_index_t orders(_self, name{scope}.value);
         auto it = orders.begin();

         while (it!=orders.end())
         {
            it = orders.erase(it);
         }
      }

      ACTION rmorder( const std::string& market,
                      const std::string& side,
                      const std::string& role,
                      uint64_t order_id){
         
         require_auth(get_self());
         check((side == MARKET_ORDER_SIDE_ASK_STR || side == MARKET_ORDER_SIDE_BID_STR), SIDE_INVALID_STR);
         check((role == MARKET_ROLE_MAKER_STR     || role == MARKET_ROLE_TAKER_STR),     ROLE_INVALID_STR);

         //要注意name类型的长度限制
         std::string scope = market + role + side;
         
         order_index_t orders(_self, name{scope}.value);
         auto it = orders.require_find(order_id,ORDER_NOT_EXIST_STR);
         orders.erase(it);
      }


      ACTION  cancelorder(const std::string& market,
                      const std::string& side,
                      const std::string& role,
                      name user,
                      uint64_t order_id){
         require_auth(user);
         check((side == MARKET_ORDER_SIDE_ASK_STR || side == MARKET_ORDER_SIDE_BID_STR), SIDE_INVALID_STR);
         check((role == MARKET_ROLE_MAKER_STR     || role == MARKET_ROLE_TAKER_STR),     ROLE_INVALID_STR);

         //要注意name类型的长度限制
         std::string scope = market + role + side;
         
         order_index_t orders(_self, name{scope}.value);

         auto it = orders.require_find(order_id,ORDER_NOT_EXIST_STR);

         check(it->status!=ORDER_STATUS_MANU_CANCEL,ERR_ORDER_CANCELED);

         if(
            (role==MARKET_ROLE_MAKER_STR && side ==MARKET_ORDER_SIDE_ASK_STR) ||
            (role==MARKET_ROLE_TAKER_STR && side ==MARKET_ORDER_SIDE_ASK_STR)
         ){

         }else{
            check(user==it->user,ERR_CANCEL_ORDER_NOT_BELONG_TO); //只能取消自己的订单
            orders.modify(it,user,[](order& o){
               o.status = ORDER_STATUS_MANU_CANCEL;
            });
         }
         
         

      }
      
      

      USING_ACTION(otcexchange,putmkorder);
      USING_ACTION(otcexchange,puttkorder);
      USING_ACTION(otcexchange,rmorder);
      USING_ACTION(otcexchange,rmorders);
      USING_ACTION(otcexchange,cancelorder);
};