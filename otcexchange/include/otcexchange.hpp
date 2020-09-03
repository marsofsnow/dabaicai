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

      ACTION hi( name nm );

      //using hi_action = action_wrapper<"hi"_n, &otcexchange::hi>;
      USING_ACTION(otcexchange,hi);




      //order操作
      ACTION putmkorder(const std::string& market,
                        name               user,
                        uint8_t            side,
                        uint64_t           price,
                        uint64_t           amount,
                        const std::string& source
                        );
                        
      ACTION puttkorder(const std::string& market,
                        name               user,
                        uint8_t            side,
                        uint64_t           price,
                        uint64_t           amount,
                        uint64_t           deal_order_id,
                        const std::string& source
                        );
      ACTION getorders(const std::string& market,uint8_t role,uint8_t side);

      USING_ACTION(otcexchange,putmkorder);
      USING_ACTION(otcexchange,puttkorder);
      USING_ACTION(otcexchange,getorders);
      



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
         time_point  ctime;                 //成交时间
         uint8_t     side;                  //taker是买还是卖
         uint64_t    deal_order_id;         //对手方订单id
         name        deal_user;             //对手方
         uint64_t    price;                 //成交价
         uint64_t    amount;                //成交数量
         uint64_t    deal_quota;            //成交额price*amount
         uint64_t    ask_fee;               //收取的卖方手续费
         uint64_t    bid_fee;               //收取的买方手续费
         std::string msg;                   //备注信息



         //表的构造函数

         deal(uint8_t side,
              uint64_t    deal_order_id,
              name        deal_user,
              uint64_t    price,
              uint64_t    amount,
              uint64_t    deal_quota,
              uint64_t    ask_fee,
              uint64_t    bid_fee,
              const std::string& msg
            ):ctime(current_time_point()),
              side(side),
              deal_order_id(deal_order_id),
              deal_user(deal_user),
              price(price),
              amount(amount),
              deal_quota(deal_quota),
              ask_fee(ask_fee),
              bid_fee(bid_fee),
              msg(msg)
              {}

      };

      TABLE order{
         uint64_t    order_id;                    //订单id，自增主键
         name        user;                        //订单所属用户，注意是name类型
         time_point  ctime;                       //订单创建时间，精确到微秒
         time_point  utime;                       //订单更新时间，精确到微秒
         uint8_t     status;                      //订单状态，吃单和挂单的状态不同
         uint8_t     side;                        //买卖类型，1卖 2买
         uint8_t     type;                        //订单类型，1限价 2市价
         uint8_t     role;                        //订单类型，1挂单 2吃单
         uint64_t    price;                       //订单交易价格
         uint64_t    amount;                      //订单交易数量
         uint64_t    min_amount;                  //订单最小成交数量
         uint64_t    taker_fee_rate;              //吃单的手续费率
         uint64_t    maker_fee_rate;              //挂单的手续费率
         uint64_t    left;                        //剩余多少数量未成交
         uint64_t    freeze;                      //冻结的stock或者money
         uint64_t    deal_fee;                    //累计的交易手续费
         uint64_t    deal_stock;                  //累计的交易sotck数量
         uint64_t    deal_money;                  //累计的交易money
         std::string source;                      //备注信息，订单来源
         std::vector<deal> vec_deal;              //成交明细


         uint64_t primary_key()          const { return order_id; }    // primary key auto increase
         uint64_t get_secondary_user()   const { return user.value;}   // sort by user name
         uint64_t get_secondary_price()  const { return price;}        // sort by price, ask increase bid of deincrease
         
      };

      using order_index_t = multi_index<"order"_n,order,
      indexed_by<"byuser"_n, const_mem_fun< order,uint64_t,&order::get_secondary_user >  >,
      indexed_by<"byprice"_n,const_mem_fun< order,uint64_t,&order::get_secondary_price > >
      > ;


     

      
      TABLE market{
         std::string         title;
         std::string         stock;
         std::string         money;
         uint64_t            stock_prec;
         uint64_t            money_prec;
         uint64_t            fee_prec;
         uint64_t            min_amount;
      };


};