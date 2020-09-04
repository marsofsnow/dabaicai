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
                  contract(self,first_receiver,ds),
                  singleton_otc(self,setting_scope.value){} //单例是属于合约的setting_scope.value

                                                                           

      ACTION hi( name nm );

      //using hi_action = action_wrapper<"hi"_n, &otcexchange::hi>;
      USING_ACTION(otcexchange,hi);




      //order操作
      ACTION putmkorder(const std::string& market,
                        const std::string& side,
                        name               user,
                        uint64_t           price,
                        uint64_t           amount,
                        const std::string& source
                        );
                        
      ACTION puttkorder(const std::string& market,
                        const std::string& side,
                        name               user,
                        uint64_t           price,
                        uint64_t           amount,
                        uint64_t           deal_order_id,
                        const std::string& source
                        );
      ACTION orderbook(const std::string& market,const std::string& role,const std::string& side);

      USING_ACTION(otcexchange,putmkorder);
      USING_ACTION(otcexchange,puttkorder);
      USING_ACTION(otcexchange,orderbook);


      //otc market 操作
      ACTION createspot(const std::string& stock,
                           const std::string& money,
                           uint8_t  stock_prec, 
                           uint8_t  money_prec, 
                           uint64_t min_amount
                            );

      
      ACTION banspot(const std::string& stock,
                        const std::string& money);

      ACTION getspots();

      ACTION removespots();


      USING_ACTION(otcexchange,createspot);
      USING_ACTION(otcexchange,banspot);
      USING_ACTION(otcexchange,getspots);
      USING_ACTION(otcexchange,removespots);



   public:
      inline static uint8_t side_to_uint(const std::string& side){
         if(side=="ask") return 1;
         if(side=="bid") return 2;
         return 0;
      }

      inline static uint8_t role_to_uint(const std::string& role){
         if(role=="mk") return 1;
         if(role=="tk") return 2;
         return 0;
      }
      //初始化单例
      inline void init_otc(){
         if(!singleton_otc.exists()){
            singleton_otc.get_or_create(_self,otc());
         }
      }
      



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
         uint8_t     status;                //成交的状态
         std::string msg;                   //备注信息

         deal() =default;

         deal(
            uint8_t side1,
            uint64_t deal_order_id1,
            name deal_user1,
            uint64_t price1,
            uint64_t amount1,
            uint64_t deal_quota1,
            uint64_t ask_fee1,
            uint64_t bid_fee1,
            uint8_t  status1,
            const std::string& msg1
         ):ctime(current_time_point()),
           side(side1),
           deal_order_id(deal_order_id1),
           deal_user(deal_user1),
           price(price1),
           amount(amount1),
           deal_quota(deal_quota1),
           ask_fee(ask_fee1),
           bid_fee(bid_fee1),
           status(status1),
           msg(msg1){}
      };


      enum class ORDER_STATUS_t : uint8_t {
         ORDER_STATUS_MAKER,
         ORDER_STATUS_MATCH,
         ORDER_STATUS_OVER
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

         std::vector<uint64_t> ids;


         uint64_t primary_key()          const { return order_id; }    // primary key auto increase
         uint64_t get_secondary_user()   const { return user.value;}   // sort by user name
         uint64_t get_secondary_price()  const { return price;}        // sort by price, ask increase bid of deincrease
         
      };
      using order_index_t = multi_index<"order"_n,
                                       order,
                                       indexed_by<"byuser"_n, const_mem_fun< order,uint64_t,&order::get_secondary_user >  >,
                                       indexed_by<"byprice"_n,const_mem_fun< order,uint64_t,&order::get_secondary_price > >
      > ;




     

      
      TABLE market{
         std::string         title;
         std::string         stock ="";
         std::string         money ="";
         uint8_t             stock_prec = 4 ; 
         uint8_t             money_prec = 2 ;
         uint8_t             fee_prec   = 4 ;
         uint64_t            min_amount = 4 ;

         


         

         market(const std::string& stock="",
                const std::string& money="",
                uint8_t  stock_prec=4, 
                uint8_t  money_prec=2, 
                uint64_t min_amount=4):
                                     title(stock+money),
                                     stock(stock),
                                     money(money),
                                     stock_prec(stock_prec),
                                     money_prec(money_prec),
                                     fee_prec(stock_prec),
                                     min_amount(min_amount){}

         EOSLIB_SERIALIZE( market, (title)(stock)(money)(stock_prec)(money_prec)(fee_prec)(min_amount) )

               
      };

      using map_market_t  =  std::map<std::string,market>;
      using set_title_t   =  std::set<std::string>; //stock money pair

      TABLE  otc{
         map_market_t map_market;
         set_title_t  set_title;
         EOSLIB_SERIALIZE( otc, ( map_market)(set_title) )
      };
      using singleton_type  = singleton< "otc"_n,otc>;
      singleton_type singleton_otc; //定义了单例类,必须显示定义合约的构造函数

      static const name setting_scope;








};