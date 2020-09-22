#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/action.hpp>
#include <eosio/transaction.hpp>
#include <eosio/singleton.hpp>

#include <eosio/name.hpp>

#include <string>

#include <vector>
#include <unordered_map>
#include "common/define.hpp"
#include "define.h"

using namespace eosio;

CONTRACT otcexchange : public contract
{
public:
   using contract::contract;
   otcexchange(name self, name first_receiver, datastream<const char *> ds) : contract(self, first_receiver, ds),
                                                                              markettable_(self, self.value)
   {
   }

   ACTION hi(name nm);

   //using hi_action = action_wrapper<"hi"_n, &otcexchange::hi>;
   USING_ACTION(otcexchange, hi);

   //------------------------define markets begin-------------------------------//
private:
   //交易对 ADXCNY
   TABLE market
   {
      symbol_code pair;     //交易对
      symbol stock;         //coin 需要指定[代币精度,代币符号]
      symbol money;         //fiat 需要指定[法币精度，法币符合]
      asset taker_fee_rate; // taker手续费费率
      asset maker_fee_rate;
      asset amount_min;                  // 限定的最小交易数量 单位是代币
      asset amount_max;                  // 限定的最大交易数量  单位是代币
      asset price_min;                   // 限定的最小交易价格  单位是法币
      asset price_max;                   // 限定的最大交易价格  单位是法币
      uint8_t status = MARKET_STATUS_ON; //交易对是否打开交易
      std::string str_status;
      time_point_sec ctime{current_time_point().sec_since_epoch()};
      time_point_sec utime{current_time_point().sec_since_epoch()};

      uint64_t primary_key() const { return pair.raw(); }
      uint64_t get_secondary_status() const { return status; } // sort by status，按交易对状态过滤

      EOSLIB_SERIALIZE(market, (pair)(stock)(money)(taker_fee_rate)(maker_fee_rate)(amount_min)(amount_max)(price_min)(price_max)(status)(str_status)(ctime)(utime))
   };

   using market_index_t = multi_index<"markettable"_n,
                                      market,
                                      indexed_by<"bystatus"_n, const_mem_fun<market, uint64_t, &market::get_secondary_status>>>;
   market_index_t markettable_; //属于合约，scope是全表

   TABLE adorder
   {
      uint64_t id;                    //广告订单id，自增主键,scope = pair+side.相当于盘口,卖盘，由小到大，买盘由大到小
      name user;                      //广告订单所属用户，注意是name类型
      symbol_code pair;               //广告订单的交易对
      time_point ctime;               //广告订单创建时间，精确到微秒
      time_point utime;               //广告订单更新时间，精确到微秒
      uint8_t status;                 //广告订单状态，吃单和挂单的状态不同
      uint8_t side;                   //买卖类型，1卖 2买
      uint8_t type;                   //广告订单类型，1限价 2市价
      uint8_t role;                   //广告订单类型，1挂单 2吃单
      asset price;                    //广告订单交易价格
      asset amount;                   //广告订单交易数量
      asset amount_min;               //广告订单最小成交数量
      asset amount_max;               //广告订单最大成交数量
      asset taker_fee_rate;           //吃单的手续费率
      asset maker_fee_rate;           //挂单的手续费率
      asset left;                     //剩余多少数量未成交
      asset freeze;                   //冻结的stock或者money,处于正在交易中,只有卖币挂单才有值
      asset deal_fee;                 //累计的交易手续费
      asset deal_stock;               //累计的交易sotck数量
      asset deal_money;               //累计的交易money
      std::string source;             //备注信息，广告订单来源
      std::vector<uint64_t> vec_deal; //成交明细，因为一个广告订单可以被很多人来交易

      uint64_t primary_key() const { return id; }                   // primary key auto increase
      uint64_t get_secondary_user() const { return user.value; }    // sort by user name，按用户名过滤
      uint64_t get_secondary_price() const { return price.amount; } // sort by price, ask increase bid of deincrease
   };
   using order_index_t = multi_index<"adordertable"_n,
                                     adorder,
                                     indexed_by<"byuser"_n, const_mem_fun<adorder, uint64_t, &adorder::get_secondary_user>>,
                                     indexed_by<"byprice"_n, const_mem_fun<adorder, uint64_t, &adorder::get_secondary_price>>

                                     >;

   //此表属于合约,scope是交易对,
   TABLE deal
   {
      uint64_t id; //deal id scope全局唯一，自增id
      name user;
      uint8_t side; //taker是买还是卖

      uint8_t type;            //广告订单类型，1限价 2市价
      uint8_t role;            //广告订单类型，1挂单 2吃单
      time_point ctime;        //创建时间
      time_point utime;        //更新时间
      name maker_user;         //对手方
      uint64_t maker_order_id; //对手方广告订单id
      asset price;             //成交价
      asset amount;            //成交数量
      asset quota;             //成交额price*amount
      asset ask_fee;           //收取的卖方手续费
      asset bid_fee;           //收取的买方手续费
      uint8_t status;          //成交的状态
      symbol_code pair;        //交易对
      std::string source;      //备注信息，仲裁信息等

      uint64_t primary_key() const { return id; }
      uint64_t get_secondary_user() const { return user.value; }
   };

   using deal_index_t = multi_index<"dealtable"_n,
                                    deal,
                                    indexed_by<"byuser"_n, const_mem_fun<deal, uint64_t, &deal::get_secondary_user>>

                                    >;

public:
   ACTION newmarket(const symbol &stock,
                    const symbol &money,
                    asset taker_fee_rate,
                    asset maker_fee_rate,
                    asset amount_min,
                    asset amount_max,
                    asset price_min,
                    asset price_max); //new one  market 操作

   ACTION closemarket(const symbol_code &pair); //交易对禁止交易
   ACTION openmarket(const symbol_code &pair);  //交易对放开交易

   //发布广告单
   ACTION putadorder(const symbol_code &pair,
                     const std::string &side,
                     name user,
                     asset price,
                     asset amount,
                     asset amount_min,
                     asset amount_max,
                     const std::string &source);

   //下架广告单

   ACTION offselfad(const symbol_code &pair,
                    const std::string &side,
                    uint64_t ad_id,
                    const std::string &reason);

   //otc 吃单
   ACTION puttkorder(const symbol_code &pair,
                     const std::string &side,
                     name user,
                     asset price,
                     asset amount,
                     uint64_t ad_id,
                     const std::string &source);
   //otc 手动和超时取消

   ACTION canceldeal(const symbol_code &pair, uint64_t deal_id, const std::string &reason);

   ACTION paydeal(const symbol_code &pair, uint64_t deal_id);  //支付
   ACTION playcoin(const symbol_code &pair, uint64_t deal_id); //放币操作

   
   inline static uint8_t side_to_uint(const std::string &side)
   {
      if (side == "ask")
         return MARKET_ORDER_SIDE_ASK;
      if (side == "bid")
         return MARKET_ORDER_SIDE_BID;
      return 0;
   }

   inline static uint8_t role_to_uint(const std::string &role)
   {
      if (role == "mk")
         return MARKET_ROLE_MAKER;
      if (role == "tk")
         return MARKET_ROLE_TAKER;
      return MARKET_ROLE_INVAID;
   }
   inline static uint8_t update_ad_status(adorder & ad, market_index_t::const_iterator itr_pair)
   {
      if (ad.status == AD_STATUS_ONTHESHELF)
      { //广告为上架中
         if (ad.left < itr_pair->amount_min)
         { //变动后交易数量小于交易对的最小交易数量时,状态改为自动下架
            ad.status = AD_STATUS_AUT_OFFTHESHELF;
         }
      }
      else if (ad.status == AD_STATUS_AUT_OFFTHESHELF) //如果是自动下架,比如吃单方撤单了
      {
         if (ad.left >= itr_pair->amount_min) //自动下架且可交易数量>=交易对配置的最小交易数量时,状态改为上架中
         {
            ad.status = AD_STATUS_ONTHESHELF; //改为上架中
         }
         else if (ad.left + ad.freeze < itr_pair->amount_min) //自动下架且可交易数量+冻结数量<交易对最小交易数量时，状态改为已完成
         {
            ad.status = AD_STATUS_FINISHED; //已完成
         }
      }

      return ad.status;
   }

   inline order_index_t get_adtable(const symbol_code &pair, const std::string &side)

   {
      check((side == MARKET_ORDER_SIDE_ASK_STR || side == MARKET_ORDER_SIDE_BID_STR), SIDE_INVALID_STR);
      std::string scope = pair.to_string() + MARKET_ROLE_MAKER_STR + side;
      transform(scope.begin(), scope.end(), scope.begin(), ::tolower); //转成小写
      return order_index_t(_self, name{scope}.value);
   }

   ACTION rmmarket(const symbol_code &pair)
   {
      require_auth(_self); //必须要求是合约账号的权限
                           //table是合约范围内的

      auto it = markettable_.find(pair.raw());

      check(it != markettable_.end(), pair.to_string() + PAIR_NOT_EXIST_STR);
      markettable_.erase(it);
      print("delete one market finish\n");
   }

   ACTION rmmarkets()
   {

      require_auth(_self); //必须要求是合约账号的权限
                           //table是合约范围内的
      auto it = markettable_.begin();
      while (it != markettable_.end())
      {
         it = markettable_.erase(it);
      }
      print("delete all market finish\n");
   }

   ACTION rmad(const symbol_code &pair, const std::string &side, uint64_t ad_id)
   {
      require_auth(get_self()); //只有合约才能删除广告订单
      auto adtable_ = get_adtable(pair, side);

      auto it = adtable_.require_find(ad_id, ORDER_NOT_EXIST_STR);
      adtable_.erase(it);
   }

   ACTION rmads(const symbol_code &pair, const std::string &side)
   {
      require_auth(get_self()); //只有合约才能删除广告订单
      auto adtable_ = get_adtable(pair, side);
      auto it = adtable_.begin();

      while (it != adtable_.end())
      {
         it = adtable_.erase(it);
      }
   }

   ACTION rmdeal(const symbol_code &pair, uint64_t id)
   {
      require_auth(get_self());
      deal_index_t dealtable_(_self, name{pair.to_string()}.value);
      auto it = dealtable_.require_find(id, "吃单找不到");
      dealtable_.erase(it);
   }

   ACTION rmdeals(const symbol_code &pair)
   {
      require_auth(get_self());
      deal_index_t dealtable_(_self, name{pair.to_string()}.value);
      auto it = dealtable_.begin();

      while (it != dealtable_.end())
      {
         it = dealtable_.erase(it);
      }
   }

   USING_ACTION(otcexchange, newmarket);
   USING_ACTION(otcexchange, closemarket);
   USING_ACTION(otcexchange, openmarket);

   USING_ACTION(otcexchange, putadorder);
   USING_ACTION(otcexchange, offselfad);

   USING_ACTION(otcexchange, puttkorder);
   USING_ACTION(otcexchange, canceldeal);

   USING_ACTION(otcexchange, rmad);
   USING_ACTION(otcexchange, rmads);

   USING_ACTION(otcexchange, rmdeal);
   USING_ACTION(otcexchange, rmdeals);

   USING_ACTION(otcexchange, rmmarket);
   USING_ACTION(otcexchange, rmmarkets);

private:
   //此表属于合约，但是有scope是用户，row就是各个币的余额(调试阶段也记录一下法币),余额表
   TABLE account
   {
      asset balance;
      uint64_t primary_key() const { return balance.symbol.code().raw(); }
   };

   //余额表历史，此表属于合约，但是scope是用户，业务调试用，
   TABLE accountlog
   {
      uint64_t id;          //自增id
      time_point_sec ctime; //创建时间
      name user;            //用户
      std::string business; //什么业务员导致了这个变化
      asset change;         //变化的值
      asset balance;        //变化后的值
      std::string detail;   //详情
      uint64_t primary_key() const { return id; }
   };
};