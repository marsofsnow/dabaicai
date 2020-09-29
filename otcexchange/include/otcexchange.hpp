#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/action.hpp>
#include <eosio/transaction.hpp>
//#include <eosio/singleton.hpp>
#include <eosio/name.hpp>

#include <string>

#include <vector>
#include <unordered_map>
//#include "common/define.hpp"
#include "define.h"

using namespace eosio;

CONTRACT otcexchange : public contract
{
public:
   using contract::contract;
   otcexchange(name self, name first_receiver, datastream<const char *> ds) : contract(self, first_receiver, ds),
                                                                              markettable_(self, self.value),
                                                                              users_(self, self.value),
                                                                              arbiters_(self, self.value),
                                                                              judgers_(self, self.value)
   {
   }

   ACTION hi(name nm);

   //using hi_action = action_wrapper<"hi"_n, &otcexchange::hi>;
   USING_ACTION(otcexchange, hi);

   //------------------------define markets begin-------------------------------//
private:
   //用户表
   TABLE user
   {
      name account;                     //EOS账户
      asset balance;                    //代币余额
      uint32_t maker_total_num;         //发布的广告数
      uint32_t maker_cancel_num;        //下架的广告数
      uint32_t taker_total_num;         //吃单数
      uint32_t taker_nopay_num;         //没有支付法币的吃单数目
      uint32_t deal_total_num;          //交易总数
      uint32_t deal_succcess_nun;       //交易成功（没有走仲裁和终审）
      uint32_t deal_arbiarate_win_num;  //仲裁获胜总数
      uint32_t deal_arbiarate_fail_num; //仲裁失败总数
      uint32_t deal_judge_win_num;      //终审成功总数
      uint32_t deal_judge_fail_num;     //终审失败总数
      uint64_t primary_key() const { return account.value; }
   };

   using user_index_t = multi_index<"usertable"_n, user>;
   user_index_t users_;

   TABLE arbiter
   {
      name account;                   //EOS账户
      asset balance;                  //代币余额
      uint32_t arbit_num;             //仲裁总场次
      uint32_t win_num;               //仲裁胜利场次
      bool is_arbit;                  //是否正在仲裁
      time_point_sec online_beg_time; //在线开始时间
      time_point_sec online_end_time; //在线失败时间
      uint64_t primary_key() const { return account.value; }
   };

   using arbiter_index_t = multi_index<"arbitertable"_n, arbiter>;
   arbiter_index_t arbiters_;

   TABLE judger
   {
      name account;      //EOS账户
      asset balance;     //代币余额
      uint32_t win_num;  //判赢总数
      uint32_t fail_num; //判输总数

      uint64_t primary_key() const { return account.value; }
   };

   using judger_index_t = multi_index<"judgertable"_n, judger>;
   judger_index_t judgers_;

   //scope是pair ,一个交易对里面的deal_id 是唯一的
   TABLE appeal
   {
      uint64_t id; //作为主键 =deal_id
      uint64_t deal_id;
      symbol_code pair;       //交易对
      uint8_t initiator_side; //发起者是买还是卖
      time_point_sec ctime;   //创建时间
      time_point_sec utime;   //更新时间
      name bid_user;          //买币的用户
      name ask_user;          //卖币的用户

      std::string bid_content;                  //买币放的申诉的内容
      std::string ask_content;                  //买币放的申诉的内容
      std::vector<std::string> bid_attachments; //买币方的附件，附件可能是url或者ipfs hash
      std::vector<std::string> ask_attachments; //卖币方的附件，附件可能是url或者ipfs hash
      std::string source;
      uint64_t primary_key() const { return id; }
      EOSLIB_SERIALIZE(appeal, (id)(deal_id)(pair)(initiator_side)(bid_user)(ask_user)(ctime)(utime)(bid_content)(ask_content)(bid_attachments)(ask_attachments)(source))
   };

   using appeal_index_t = multi_index<"appealtable"_n, appeal>;

   //scope 是pair，仲裁订单
   TABLE arborder
   {
      uint64_t id;          //仲裁id,实际==deal id
      time_point_sec ctime; //创建时间
      time_point_sec utime; //更新时间
      symbol_code pair;     //交易对
      uint64_t appeal_id;   //申诉材料id
      uint64_t deal_id;
      uint8_t initiator_side;                  //发起者是买还是卖
      asset amount;                            //仲裁数量
      asset price;                             //仲裁价格
      asset quota;                             //终金额
      std::vector<name> vec_arbiter;           //仲裁员成员
      std::map<name, uint8_t> map_person_pick; //每个人的仲裁结果
      asset arbitrate_fee;                     //仲裁手续费

      uint16_t person_num; //仲裁员人数
      uint16_t yes_num;    //同意放币
      uint16_t no_num;     //取消
      uint8_t status;      //仲裁进度
      std::string source;

      uint64_t primary_key() const { return id; }
      EOSLIB_SERIALIZE(arborder, (id)(ctime)(utime)(pair)(appeal_id)(deal_id)(initiator_side)(amount)(price)(quota)(vec_arbiter)(map_person_pick)(arbitrate_fee)(person_num)(yes_num)(no_num)(status)(source))
   };

   using arborder_index_t = multi_index<"arbordertable"_n, arborder>;

   ///每个人的仲裁记录就是deal了,
   //scope 是每个用户，也就是说这个表保存的是一个用户的数据

   TABLE arblog
   {
      uint64_t id;          //自增主键
      time_point_sec ctime; //创建时间
      time_point_sec utime; //更新时间

      symbol_code pair;
      uint64_t arbitrate_id; //仲裁id
      uint64_t deal_id;      //订单id
      uint64_t appeal_id;    //申诉id

      uint8_t self_arbit;  //我的仲裁结果
      uint8_t final_arbit; //实际仲裁结果

      asset labor_fee;    //劳务费
      uint8_t status;     //1.仲裁收入 2.待结算 午夜12点结算 3.恶意仲裁 扣押金
      std::string source; //备注
   };

   using arblog_index_t = multi_index<"arblogtable"_n, arblog>;

   //上传材料和申诉
   ACTION putappeal(name who,
                    const std::string &side,
                    const symbol_code &pair,
                    uint64_t deal_id,
                    const std::string &content,
                    const std::vector<std::string> attachments,
                    const std::string &source)
   {
      require_auth(who);

      check((side == MARKET_ORDER_SIDE_ASK_STR || side == MARKET_ORDER_SIDE_BID_STR), ERR_MSG_SIDE);
      check(!content.empty(), "伸诉内容不能为空");
      check(!content.empty(), "伸诉上传附件不能为空");

      //查看deal_id 是不是存在
      deal_index_t dealtable_(_self, name{pair.to_string()}.value);
      auto side_uint = side_to_uint(side);
      auto itr_deal = dealtable_.require_find(deal_id, "要申诉的deal不存在");
      check(itr_deal->status == DEAL_STATUS_PAID_WAIT_PLAYCOIN, "只有处在已支付待放币状态的deal 才允许买卖双方发起申诉");
      if (who == itr_deal->user) //taker上传材料
      {
         check(itr_deal->side == side_uint, "who 作为taker，side 出错");
      }
      else if (who == itr_deal->maker_user)
      {
         auto maker_side = (itr_deal->side == MARKET_ORDER_SIDE_ASK) ? MARKET_ORDER_SIDE_BID : MARKET_ORDER_SIDE_ASK;
         check(maker_side == side_uint, "who 作为maker，side 出错");
      }
      else
      {
         print("who 不存在");
         return;
      }

      //先看是否存在这个申诉
      appeal_index_t appeals(_self, name{pair.to_string()}.value);
      auto itr_appeal = appeals.find(deal_id);
      if (itr_appeal == appeals.end())
      {
         //要创建一个deal
         appeals.emplace(_self, [&](appeal &a) {
            a.id = deal_id;
            a.pair = pair;
            a.initiator_side = side_uint;
            a.ctime = time_point_sec(current_time_point().sec_since_epoch());
            a.utime = a.ctime;

            if (side_uint == MARKET_ORDER_SIDE_ASK)
            {
               a.ask_user = who;
               a.ask_content = content;
               a.ask_attachments = attachments;
            }
            else
            {
               a.bid_user = who;
               a.bid_content = content;
               a.bid_attachments = attachments;
            }
            a.source.append(source);
         });
         dealtable_.modify(itr_deal, _self, [&side_uint](deal &d) {
            d.status = (side_uint == MARKET_ORDER_SIDE_ASK) ? DEAL_STATUS_PAID_APPEAL_ASK : DEAL_STATUS_PAID_APPEAL_BID;
         });

         //新增一个申诉，这个时候要生成一个仲裁订单，然后这个订单下面又有很多仲裁log，来记录仲裁员的仲裁记录
         /*
         *  uint64_t id;          //仲裁id,实际==deal id
            time_point_sec ctime; //创建时间
            time_point_sec utime; //更新时间
            symbol_code pair;     //交易对
            uint64_t appeal_id;   //申诉材料id
            uint64_t deal_id;
            uint8_t initiator_side;                  //发起者是买还是卖
            asset amount;                            //仲裁数量
            asset price;                             //仲裁价格
            asset quota;                             //终金额
            std::vector<name> vec_arbiter;           //仲裁员成员
            std::map<name, uint8_t> map_person_pick; //每个人的仲裁结果
            asset arbitrate_fee;                     //仲裁手续费

            uint16_t person_num; //仲裁员人数
            uint16_t yes_num;    //同意放币
            uint16_t no_num;     //取消
            uint8_t status;      //仲裁进度
            std::string source;
         */

         //算出哪些可以当做仲裁员

         std::vector<name> arbiters;
         //在线时间
         //有无正在进行的仲裁订单
         //仲裁胜利场数
         //仲裁胜利率 =仲裁胜利场次/仲裁总场次

         arborder_index_t arbs(_self, name{pair.to_string()}.value); //订单表

         auto itr_pair = get_market(pair);

         arbs.emplace(_self, [&](arborder &a) {
            a.id = deal_id;
            a.ctime = time_point_sec(current_time_point().sec_since_epoch());
            a.utime = a.ctime;
            a.pair = pair;
            a.appeal_id = deal_id;
            a.deal_id = deal_id;
            a.initiator_side = side_uint;
            a.amount = itr_deal->amount;
            a.price = itr_deal->price;
            a.quota = itr_deal->quota;
            a.arbitrate_fee = itr_pair->zero_stock;
            a.vec_arbiter = arbiters;
            a.person_num = a.vec_arbiter.size();
            a.yes_num = 0;
            a.no_num = 0;

            a.status = ARBDEAL_STATUS_CREATED;
            a.source.append(source);
         });

         //往每个仲裁员仲裁log表里投递任务
         /*
         * uint64_t id;          //自增主键
         time_point_sec ctime; //创建时间
         time_point_sec utime; //更新时间

         symbol_code pair;
         uint64_t arbitrate_id; //仲裁id
         uint64_t deal_id;      //订单id
         uint64_t appeal_id;    //申诉id

         uint8_t self_arbit;  //我的仲裁结果
         uint8_t final_arbit; //实际仲裁结果

         asset labor_fee;    //劳务费
         uint8_t status;     //1.仲裁收入 2.待结算 午夜12点结算 3.恶意仲裁 扣押金
         std::string source; //备注
         */

         for (auto arbiter : arbiters)
         {
            arblog_index_t arblogs(_self, arbiter.value); //表名，按用户分表，不是按pair分表
            arblogs.emplace(_self, [&](arblog &log) {
               log.id = arblogs.available_primary_key();
               log.ctime = time_point_sec(current_time_point().sec_since_epoch());
               log.utime = log.ctime;
               log.pair = pair;
               log.arbitrate_id = deal_id;
               log.deal_id = deal_id;
               log.appeal_id = deal_id;
               log.self_arbit = ARBIT_UNKOWN;
               log.final_arbit = ARBIT_UNKOWN;
               log.labor_fee = itr_pair->zero_stock;
               log.status = ARBDEAL_STATUS_CREATED;
               log.source = source;
            });
         }
      }
      else //对手房已经上传过申诉材料
      {
         check((itr_appeal->initiator_side == MARKET_ORDER_SIDE_ASK && side_uint == MARKET_ORDER_SIDE_BID) ||
                   (itr_appeal->initiator_side == MARKET_ORDER_SIDE_BID && side_uint == MARKET_ORDER_SIDE_ASK),
               "who side invaild");
         //说明之前已经有过申诉，这个应该是对手方的申诉
         appeals.modify(itr_appeal, _self, [&](appeal &a) {
            a.utime = time_point_sec(current_time_point().sec_since_epoch());
            if (itr_appeal->initiator_side == MARKET_ORDER_SIDE_ASK)
            {
               a.bid_user = who;
               a.bid_content = content;
               a.bid_attachments = attachments;
            }
            else
            {
               a.ask_user = who;
               a.ask_content = content;
               a.ask_attachments = attachments;
            }
            a.source.append("对手方上传申诉材料|").append(source);
         });

         dealtable_.modify(itr_deal, _self, [&side_uint](deal &d) {
            d.status = DEAL_STATUS_PAID_APPEAL_ALL;
         });
      }
   }

   //交易对 ADXCNY
   TABLE market
   {
      symbol_code pair;                  //交易对
      symbol stock;                      //coin 需要指定[代币精度,代币符号]
      symbol money;                      //fiat 需要指定[法币精度，法币符合]
      asset taker_fee_rate;              // taker手续费费率[数量，代币精度，代币符号]
      asset maker_fee_rate;              // maker手续费费率[数量，代币精度，代币符号]
      asset amount_min;                  // 限定的最小交易数量  [数量，代币精度，代币符号]
      asset amount_max;                  // 限定的最大交易数量  [数量，代币精度，代币符号]
      asset price_min;                   // 限定的最小交易价格  [数量，代币精度，代币符号]
      asset price_max;                   // 限定的最大交易价格  [数量，代币精度，代币符号]
      asset zero_stock;                  //代币的0值
      asset zero_money;                  //法币的0值
      uint32_t pay_timeout;              //买币方支付法币的超时时间,精确到s
      uint32_t self_playcoin_time;       //自己放币延迟时间
      uint32_t def_cancel_time;          //初次仲裁取消，延迟取消的时间,精确到s
      uint32_t def_playcoin_time;        //初次仲裁通过，延迟放币的时间,精确到s
      uint32_t cancel_ad_num;            //允许用户取消广告单次数
      uint8_t status = MARKET_STATUS_ON; //交易对是否允许交易
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

   //广告挂单
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

      uint8_t type;                          //广告订单类型，1限价 2市价
      uint8_t role;                          //广告订单类型，1挂单 2吃单
      time_point ctime;                      //创建时间
      time_point utime;                      //更新时间
      name maker_user;                       //对手方
      uint64_t maker_order_id;               //对手方广告订单id
      asset price;                           //成交价
      asset amount;                          //成交数量
      asset quota;                           //成交额price*amount
      asset ask_fee;                         //收取的卖方手续费
      asset bid_fee;                         //收取的买方手续费
      uint8_t status;                        //成交的状态
      uint32_t pay_timeout;                  //支付超时时间,
      uint64_t pay_timeout_sender_id;        //异步事物id
      uint64_t self_playcoin_sender_id;      //异步事物id
      uint64_t arbiarate_cancel_sender_id;   //异步事物idARBIARATE
      uint64_t arbiarate_playcoin_sender_id; //异步事物id
      symbol_code pair;                      //交易对
      std::string source;                    //备注信息，仲裁信息等

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
                    asset price_max,
                    uint32_t pay_timeout,
                    uint32_t self_playcoin_time,
                    uint32_t def_cancel_timeout,
                    uint32_t def_playcoin_timeout,
                    uint8_t cancel_ad_num); //new one  market 操作

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

   void rollbackdeal(const symbol_code &pair,
                     name who,
                     uint64_t deal_id,
                     uint8_t status,
                     const std::string &reason);

   void rollbackdeal_v1(const symbol_code &pair,
                        name who,
                        deal_index_t &dealtable_,
                        deal_index_t::const_iterator itr_deal,
                        uint8_t status,
                        const std::string &reason);
   void commitdeal_v1(const symbol_code &pair,
                      name who,
                      deal_index_t &dealtable_,
                      deal_index_t::const_iterator itr_deal,
                      uint8_t status,
                      const std::string &reason); //放币操作
   void commitdeal(const symbol_code &pair,
                   name who,
                   uint64_t deal_id,
                   uint8_t status,
                   const std::string &reason);

   //1.手动取消待支付的订单
   //手动取消：买币方待支付->待支付取消完成
   //超时取消：买币方待支付->超时待支付取消
   //仲裁取消：仲裁中->仲裁取消中->取消完成
   //终审取消：仲裁放币/取消->终审取消中->取消完成

   ACTION mancldeal(const symbol_code &pair, name who, uint64_t deal_id, const std::string &reason)
   {
      //
      require_auth(who);

      rollbackdeal(pair, who, deal_id, DEAL_STATUS_UNPAID_MAN_CANCEL, reason);
   }

   //3.买币方支付法币
   ACTION paydeal(const symbol_code &pair, uint64_t deal_id);

   //4.卖币方主动放币
   ACTION selfplaycoin(const symbol_code &pair, name who, uint64_t deal_id, const std::string &reason) //放币操作
   {
      require_auth(who);

      //
      deal_index_t dealtable_(_self, name{pair.to_string()}.value);
      auto it = dealtable_.require_find(deal_id, "吃单找不到");
      check(it->status == DEAL_STATUS_PAID_WAIT_PLAYCOIN, "只有买方支付了，才能放币");

      dealtable_.modify(it, _self, [&](deal &d) {
         d.status = DEAL_STATUS_PAID_PLAYCOIN_ING; //把状态改为放币中
      });

      //是否立即放币
      bool right_now = true;
      if (right_now)
      {
         commitdeal_v1(pair, who, dealtable_, it, DEAL_STATUS_PAID_PLAYCOIN_FINISH, reason);
      }
      else
      { //延迟事物放币
         auto now = current_time_point().sec_since_epoch();
         dealtable_.modify(it, _self, [&](deal &d) {
            d.self_playcoin_sender_id = now;
         });

         //放币XX后生效

         transaction t{};

         t.actions.emplace_back(
             permission_level(_self, name{"active"}),
             _self,
             name{"defcmdeal"},
             std::make_tuple(pair, who, deal_id, DEAL_STATUS_PAID_PLAYCOIN_FINISH, "放币中"));

         auto itr_pair = get_market(pair);

         t.delay_sec = itr_pair->self_playcoin_time;

         t.send(now, _self);

         //print_f("Sent with a delay of %d ", t.delay_sec);
      }
   }

   //异步取消订单,异步延迟事物
   ACTION defcldeal(const symbol_code &pair, name who, uint64_t deal_id, uint8_t status, const std::string &reason)
   {
      //权限是合约
      require_auth(get_self());
      rollbackdeal(pair, who, deal_id, status, reason);
   }

   ACTION defcmdeal(const symbol_code &pair, name who, uint64_t deal_id, uint8_t status, const std::string &reason)
   {
      //权限是合约
      require_auth(get_self());

      commitdeal(pair, who, deal_id, status, reason);
   }

   //-------------------------------------------

   //仲裁者仲裁取消deal
   //手动取消：买币方待支付->待支付取消完成
   //超时取消：买币方待支付->超时待支付取消
   //仲裁取消：仲裁中->仲裁取消中->取消完成
   ACTION artrbdeal(name arbiter, const symbol_code &pair, name who, uint64_t deal_id, const std::string &reason)
   {
      //判定这个单被取消了
      require_auth(arbiter);

      //修改订单的状态为 DEAL_STATUS_PAID_ARBIARATE_CANCEL

      //立即把订单状态更改为仲裁取消状态，但是只是记录交易更改，实际的取消要延迟
      deal_index_t dealtable_(_self, name{pair.to_string()}.value);
      auto it = dealtable_.require_find(deal_id, "吃单找不到");
      check(it->status == DEAL_STATUS_PAID_ARBIARATE_ING, "吃单发起伸述了，才能仲裁被取消");

      auto now = current_time_point().sec_since_epoch();
      dealtable_.modify(it, _self, [&](deal &d) {
         d.status = DEAL_STATUS_PAID_ARBIARATE_CANCEL; //立即更改状态使用户看的到
         d.arbiarate_cancel_sender_id = now;
         d.source.append(reason); //你的订单已经被仲裁取消，XX后生效，如有异议，请发起终审
      });

      //取消XX后生效,状态DEAL_STATUS_PAID_ARBIARATE_CANCEL->DEAL_STATUS_CANCEL_FINISH

      transaction t{};

      t.actions.emplace_back(
          permission_level(_self, name{"active"}),
          _self,
          name{"defcldeal"},
          std::make_tuple(pair, who, deal_id, DEAL_STATUS_CANCEL_FINISH, "deal因仲裁取消但是用户又没有发起终审而取消"));

      auto itr_pair = get_market(pair);

      t.delay_sec = itr_pair->pay_timeout;

      t.send(now, _self);

      ////print_f("Sent with a delay of %d ", t.delay_sec);
   }

   //仲裁者仲裁提交deal
   ACTION artcmdeal(name arbiter, const symbol_code &pair, name who, uint64_t deal_id, const std::string &reason)
   {

      //被仲裁放币
      require_auth(arbiter);

      //修改订单的状态为 DEAL_STATUS_PAID_ARBIARATE_CANCEL

      deal_index_t dealtable_(_self, name{pair.to_string()}.value);
      auto it = dealtable_.require_find(deal_id, "吃单找不到");

      check(it->status == DEAL_STATUS_PAID_ARBIARATE_ING, "吃单发起伸述了，才能仲裁放币");

      auto now = current_time_point().sec_since_epoch();
      dealtable_.modify(it, _self, [&](deal &d) {
         d.status = DEAL_STATUS_PAID_ARBIARATE_PALYCOIN; //立即更改状态使用户看的到
         d.arbiarate_playcoin_sender_id = now;
         d.source.append(reason); //你的订单已经被仲裁放币，XX后生效，如有异议，请发起终审
      });

      //放币XX后生效

      transaction t{};

      t.actions.emplace_back(
          permission_level(_self, name{"active"}),
          _self,
          name{"defcmdeal"},
          std::make_tuple(pair, who, deal_id, DEAL_STATUS_PAID_PLAYCOIN_FINISH, "放币中"));

      auto itr_pair = get_market(pair);

      t.delay_sec = itr_pair->pay_timeout;

      t.send(now, _self);

      ////print_f("Sent with a delay of %d ", t.delay_sec);
   }

   //手动取消：买币方待支付->待支付取消完成
   //超时取消：买币方待支付->超时待支付取消
   //仲裁取消：仲裁中->仲裁取消中->取消完成
   //终审取消：仲裁取消或者放币->终审取消->取消完成
   ACTION judgerbdeal(name judger, const symbol_code &pair, name who, uint64_t deal_id, const std::string &reason)
   {
      //终审判定这个单被取消了
      require_auth(judger);

      //修改订单的状态为 DEAL_STATUS_PAID_ARBIARATE_CANCEL

      deal_index_t dealtable_(_self, name{pair.to_string()}.value);
      auto it = dealtable_.require_find(deal_id, "吃单找不到");

      check(it->status == DEAL_STATUS_PAID_ARBIARATE_CANCEL || it->status == DEAL_STATUS_PAID_ARBIARATE_PALYCOIN, "只有经过仲裁了才能发起终审");

      uint8_t old_status = it->status;

      auto now = current_time_point().sec_since_epoch();
      dealtable_.modify(it, _self, [&](deal &d) {
         d.status = DEAL_STATUS_PAID_JUDGE_CANCEL; //立即更改状态使用户看的到
         d.source.append(reason);                  //你的订单已经被仲裁取消，XX后生效，如有异议，请发起终审
      });

      uint64_t def_sender_id = 0;

      //维持原判
      if (old_status == DEAL_STATUS_PAID_ARBIARATE_CANCEL)
      {
         def_sender_id = it->arbiarate_cancel_sender_id;
      }
      else if (old_status == DEAL_STATUS_PAID_ARBIARATE_PALYCOIN)
      {
         def_sender_id = it->arbiarate_playcoin_sender_id;
      }

      int rescode = cancel_deferred(def_sender_id);
      if (rescode == 1)
      {
         rollbackdeal(pair, who, deal_id, DEAL_STATUS_CANCEL_FINISH, reason); //把订单状态改为取消完成
      }
      else
      {
         print("之前的仲裁取消放币或者放币异步事务找不到了，重大bug，测试要注意");
      }
   }

   //自己放币：已付款待支付中->放币中->放币完成
   //仲裁放币：仲裁中->仲裁放币中->放币完成
   //终审放币：仲裁取消或放币->终审放币中->放币完成

   //终审者提交订单
   ACTION judgecmdeal(name judger, const symbol_code &pair, name who, uint64_t deal_id, const std::string &reason)
   {
      //终审判定这个单被取消了
      require_auth(judger);

      //修改订单的状态为 DEAL_STATUS_PAID_ARBIARATE_CANCEL

      deal_index_t dealtable_(_self, name{pair.to_string()}.value);
      auto it = dealtable_.require_find(deal_id, "吃单找不到");

      check(it->status == DEAL_STATUS_PAID_ARBIARATE_CANCEL || it->status == DEAL_STATUS_PAID_ARBIARATE_PALYCOIN, "只有经过仲裁了才能发起终审");

      uint8_t old_status = it->status;

      auto now = current_time_point().sec_since_epoch();
      dealtable_.modify(it, _self, [&](deal &d) {
         d.status = DEAL_STATUS_PAID_JUDGE_PLAYCOIN; //立即更改状态使用户看的到
         d.source.append(reason);                    //你的订单已经被仲裁取消，XX后生效，如有异议，请发起终审
      });

      uint64_t def_sender_id = 0;

      //维持原判
      if (old_status == DEAL_STATUS_PAID_ARBIARATE_CANCEL)
      {
         def_sender_id = it->arbiarate_cancel_sender_id;
      }
      else if (old_status == DEAL_STATUS_PAID_ARBIARATE_PALYCOIN)
      {
         def_sender_id = it->arbiarate_playcoin_sender_id;
      }

      int rescode = cancel_deferred(def_sender_id);
      if (rescode == 1)
      {
         commitdeal_v1(pair, who, dealtable_, it, DEAL_STATUS_PAID_PLAYCOIN_FINISH, reason);
      }
      else
      {
         print("之前的仲裁取消放币或者放币异步事务找不到了，重大bug，测试要注意");
      }
   }

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
      //每次广告可交易数量变动后,对广告进行判断
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

   inline order_index_t::const_iterator get_adorder(const symbol_code &pair, const std::string &side, uint64_t ad_id)
   {

      auto adtable_ = get_adtable(pair, side);
      auto it = adtable_.require_find(ad_id, ERR_MSG_AD_NOT_EXIST);
      return it;
   }

   inline market_index_t::const_iterator get_open_market(const symbol_code &pair) //
   {
      auto it = markettable_.require_find(pair.raw(), ERR_MSG_CHECK_FAILED(50110, pair.to_string().append(ERR_MSG_PAIR_NOT_EXIST)).c_str());
      check(it->status == MARKET_STATUS_ON, ERR_MSG_CHECK_FAILED(50111, pair.to_string().append(ERR_MSG_PAIR_NOT_ALLOW_TRADE)));
      return it;
   }

   inline market_index_t::const_iterator get_market(const symbol_code &pair) //
   {
      auto it = markettable_.require_find(pair.raw(), ERR_MSG_CHECK_FAILED(50110, pair.to_string().append(ERR_MSG_PAIR_NOT_EXIST)).c_str());
      return it;
   }

   inline void transfer(name from,
                        name to,
                        asset quantity,
                        std::string memo)
   {
      //可以冻结,转账,扣手续费等
      action(
          permission_level{from, name{"active"}},
          name{TOKEN_CONTRACT_NAME},
          name{TOKEN_CONTRACT_TRANSFER_ACTION},
          std::make_tuple(from, to, quantity, memo))
          .send();
   }

   //on_notify函数不是可供直接调用的合约函数，它是一个通知处理程序（notification handler）必须指明监听的合约名（code）和Action名（action），这样可以避免处理从非eosio.token合约发送来的“假EOS”，提高了安全性
   //on_notify函数能够监听eosio.token::transfer，是因为eosio.token::transfer内部调用了require_recipient(to)函数
   //on_notify属性接受通配符，比如这样定义：[[eosio::on_notify("*::transfer")]] 可以让on_notify函数处理任何合约的transfer Action。
   [[eosio::on_notify("eosio.token::transfer")]] void onTransfer(name from, name to, asset quantity, std::string memo)
   {
      if (to != _self)
         return;
      eosio::check(quantity.symbol == symbol("EOS", 4), "must pay with EOS token");
      eosio::check(quantity.amount > 10000, "must send more than 1 EOS");

      asset new_quantity = asset(quantity.amount + 1, symbol("EOS", 4));
      action(permission_level{_self, "active"_n},
             "eosio.token"_n,
             "transfer"_n,
             make_tuple(_self, from, new_quantity, std::string("+0.0001 EOS!")))
          .send();
   }
   //法币支付超时->超时取消
   ACTION paytimeout(const symbol_code &pair, uint64_t deal_id, const std::string &message)
   {
      print("Printing deferred ", pair, deal_id, message);
   }

   //如果延期交易执行失败，EOSIO会发送一个eosio::onerror的Action，可以使用on_notify监听此Action，从onerror对象获取sender_id和打包后的transaction数据，进行错误处理
   [[eosio::on_notify("eosio::onerror")]] void onError(const onerror &error)
   {

      print("Resending Transaction: ", error.sender_id);
      transaction dtrx = error.unpack_sent_trx();
      dtrx.delay_sec = 3;

      //dtrx.send(eosio::current_time_point().sec_since_epoch(), _self);
   }

   ACTION rmmarket(const symbol_code &pair)
   {
      require_auth(_self); //必须要求是合约账号的权限+table是合约范围内的
      auto it = get_open_market(pair);
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
      auto it = adtable_.require_find(ad_id, ERR_MSG_AD_NOT_EXIST);
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