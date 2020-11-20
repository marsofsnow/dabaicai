#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/action.hpp>
#include <eosio/transaction.hpp>
#include <eosio/singleton.hpp>
#include <eosio/name.hpp>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include "define.h"

using namespace eosio;

CONTRACT otcexchange : public contract
{
public:
   using contract::contract;
   otcexchange(name self, name first_receiver, datastream<const char *> ds) : contract(self, first_receiver, ds),
                                                                              markets_(self, self.value),
                                                                              overview_(self, self.value),
                                                                              log_(self, _self.value),
                                                                              arbsts_(self, _self.value),
                                                                              arbpools_(self, _self.value)

   {
      overview ow;
      overview_.get_or_create(_self, ow);
   }

   ACTION putadorder(const symbol_code &pair,
                     const std::string &side,
                     name user,
                     asset price,
                     asset amount,
                     asset amount_min,
                     asset amount_max,
                     const std::vector<uint64_t> &pay_accounts,
                     const std::vector<uint64_t> &pay_ways,
                     const std::vector<std::string> &pay_account_contexts,
                     const std::string &source);

   ACTION offad(const symbol_code &pair,
                const std::string &side,
                uint64_t ad_id,
                const std::string &reason);

   ACTION puttkorder(const symbol_code &pair,
                     const std::string &side,
                     name user,
                     asset price,
                     asset amount,
                     uint64_t ad_id,
                     const std::vector<uint64_t> &pay_accounts,
                     const std::vector<uint64_t> &pay_ways,
                     const std::vector<std::string> &pay_account_contexts,
                     const std::string &source);

   ACTION mancldeal(const symbol_code &pair, name bider, uint64_t deal_id, const std::string &reason);

   ACTION paydeal(const symbol_code &pair, name bider, uint64_t deal_id, uint64_t fiat_pay_method, uint64_t pmt_id);

   ACTION selfplaycoin(const symbol_code &pair, name asker, uint64_t deal_id, const std::string &reason, bool right_now);

   ACTION putappeal(name who,
                    const std::string &side,
                    const symbol_code &pair,
                    uint64_t deal_id,
                    const std::vector<std::string> &vec_contacts,
                    const std::string &reason,
                    const std::string &desc,
                    const std::vector<std::string> &vec_img,
                    const std::vector<std::string> &vec_video,
                    const std::string &source);

   ACTION putjudge(name failer, const symbol_code &pair, uint64_t deal_id, const std::vector<std::string> &vec_contacts,
                   const std::string &reason,
                   const std::string &desc,
                   const std::vector<std::string> &vec_img,
                   const std::vector<std::string> &vec_video,
                   const std::string &source);

   ACTION defcldeal(const symbol_code &pair, name cmder, uint64_t deal_id, uint8_t status, const std::string &reason);

   ACTION defcmdeal(name cmder, const symbol_code &pair, name asker, uint64_t deal_id, uint8_t status, const std::string &reason);
   ACTION cmddefcmdeal(name cmder, const symbol_code &pair, name asker, uint64_t deal_id, uint8_t status, const std::string &reason);

   ACTION arbdeal(name arbiter, const symbol_code &pair, uint64_t deal_id, uint8_t choice, const std::string &reason);
   ACTION judgedeal(name judger, const symbol_code &pair, uint64_t deal_id, uint8_t choice, const std::string &reason);
   ACTION moddealss(const symbol_code &pair, uint64_t deal_id, uint8_t status, const std::string &reason);

   ACTION hi(name nm);

   //仲裁员的档位
   ACTION newarbst(const symbol_code &stock, const std::vector<asset> &vec_level);
   ACTION rmarbst(const symbol_code &stock);
   ACTION rmarbsts();
   ACTION rmdhs(name who);

   //申请成为仲裁员
   ACTION regarbiter(name arbitername, const symbol_code &stock, asset mortgage, uint32_t online_begin, uint32_t online_end, const std::string &email, const std::string &verify_code);
   //解除仲裁员
   ACTION unregarbiter(name arbitername, const symbol_code &stock, const std::string &reason);

   ACTION regjuder(name judgername, const symbol_code &stock, uint32_t online_begin, uint32_t online_end, const std::string &email, const std::string &verify_code);
   ACTION unregjudger(name judgername, const symbol_code &stock, const std::string &reason);
   //删除中裁员
   ACTION rmarbiters(const symbol_code &stock);
   ACTION rmjudgers(const symbol_code &stock);
   //删除仲裁记录
   ACTION rmappeals(const symbol_code &pair);
   ACTION rmarborders(const symbol_code &pair);
   ACTION rmarbtasks(name arbitername);

   ACTION rmjudges(const symbol_code &pair);
   ACTION rmjudorders(const symbol_code &pair);
   ACTION rmjudtasks(name judgername);

   //using hi_action = action_wrapper<"hi"_n, &otcexchange::hi>;
   //USING_ACTION(otcexchange, hi);
   ACTION newmarket(const symbol &stock,
                    const symbol &money,
                    double taker_ask_fee_rate,
                    double taker_bid_fee_rate,
                    double maker_ask_fee_rate,
                    double maker_bid_fee_rate,
                    asset amount_min,
                    asset amount_max,
                    asset price_min,
                    asset price_max,
                    uint32_t pay_timeout,
                    uint32_t self_playcoin_time,
                    uint32_t def_cancel_timeout,
                    uint32_t def_playcoin_timeout,
                    uint8_t cancel_ad_num);

   ACTION closemarket(const symbol_code &pair);
   ACTION openmarket(const symbol_code &pair);

   ACTION rmmarket(const symbol_code &stock, const symbol_code &money);

   ACTION rmmarkets();
   ACTION rmad(const symbol_code &pair, const std::string &side, uint64_t ad_id);
   ACTION rmads(const symbol_code &pair, const std::string &side);
   ACTION rmdeal(const symbol_code &pair, uint64_t id);
   ACTION rmdeals(const symbol_code &pair);
   ACTION canmodpay(name user);
   ACTION rmnotfills();
   ACTION moduserrole(name user, const symbol_code &stock, uint8_t role);
   ACTION erauserrole(name user, const symbol_code &stock, uint8_t role);
   ACTION cleartables();

private:
   //余额表历史，此表属于合约，但是scope是用户，业
   TABLE balancelog
   {
      uint64_t id;          //自增id
      time_point_sec ctime; //创建时间
      name user;            //scope
      symbol_code stock;    //二级索引

      std::string business; //什么业务员导致了这个变化
      asset amount;
      asset fee;
      asset change;       //变化的值
      asset balance;      //变化后的值
      std::string detail; //详情
      uint64_t primary_key() const { return id; }
      uint64_t bystock() const { return stock.raw(); }
   };

   using balancelog_index_t = multi_index<"balancelogs"_n,
                                          balancelog,
                                          indexed_by<"bystock"_n, const_mem_fun<balancelog, uint64_t, &balancelog::bystock>>

                                          >;

   void inset_balancelog(name user, const symbol_code &stock, const std::string &business, asset amount, asset fee, asset change, asset balance, const std::string &detail)
   {
      balancelog_index_t t(_self, user.value); //得到表
      t.emplace(_self, [&](balancelog &b) {
         b.id = t.available_primary_key();
         b.ctime = CURRENT_SEC;
         b.user = user;
         b.stock = stock;
         b.business = business;
         b.amount = amount;
         b.fee = fee;
         b.change = change;
         b.balance = balance;
         b.detail = detail;
      });
   }

   void erase_balancelogs(name user)
   {
      balancelog_index_t t(_self, user.value); //得到表
      auto it = t.begin();
      while (it != t.end())
      {
         it = t.erase(it);
      }
   }

   TABLE logitem
   {
      uint64_t id; //时间id
      time_point_sec ctime;
      std::string msg;
      uint64_t primary_key() const { return id; }
   };
   using log_index_t = multi_index<"log"_n, logitem>;
   log_index_t log_;

   void erase_logs()
   {
      auto it = log_.begin();
      while (it != log_.end())
      {
         it = log_.erase(it);
      }
   }

   //仲裁池，scope是合约，主键是代币
   TABLE arbpool
   {
      symbol_code stock;   //代币作为主键
      asset mortgage;      //总的质押总金额
      asset total_arb_fee; //总的仲裁手续费
      asset today_win_num;
      asset today_arb_fee;
      time_point_sec ctime;
      time_point_sec utime;
      std::string source; //备注
      uint64_t primary_key() const { return stock.raw(); }
   };
   using arbpool_index_t = multi_index<"arbpools"_n, arbpool>;
   arbpool_index_t arbpools_;

   //仲裁员的相关配置，scope是合约，主键是代币
   TABLE arbst
   {
      symbol_code stock;            //主键
      std::vector<asset> vec_level; //质押代币的等级
      time_point_sec ctime;
      time_point_sec utime;
      uint64_t primary_key() const { return stock.raw(); }
   };
   using arbst_index_t = multi_index<"arbsts"_n, arbst>;
   arbst_index_t arbsts_;

   //===================用户的单日统计计数,决定了用户是不是进黑名单,超过阀值了,自动进黑名单。scope是用户，主键是日期，精确到00点秒============
   TABLE userdaystat
   {
      time_point_sec day;
      name account;
      uint32_t day_cancel_deal_num = 0; //单日取消deal的次数
      uint32_t day_cancel_ad_num = 0;   //单日取消广告的次数
      uint32_t day_arb_fail_num = 0;    //单日仲裁失败次数
      std::string source{""};

      uint64_t primary_key() const { return day.sec_since_epoch(); }
   };

   using userdaystat_index_t = multi_index<"userdaystats"_n, userdaystat>;

   void add_cancel_deal_num_once(name who, time_point_sec when, const std::string &reason)
   {
      userdaystat_index_t t(_self, who.value);
      auto it = t.find(day_start(when).sec_since_epoch());
      if (it == t.end())
      {
         t.emplace(_self, [&](userdaystat &u) {
            u.day_cancel_deal_num = 1;
            u.source.append("|").append(reason);
         });
      }
      else
      {
         t.modify(it, _self, [&](userdaystat &u) {
            u.day_cancel_deal_num++;
            u.source.append("|").append(reason);
         });
      }
   }

   bool can_day_deal(name who, time_point_sec when)
   {
      userdaystat_index_t t(_self, who.value);
      auto it = t.find(day_start(when).sec_since_epoch());
      bool res = true;
      if (it != t.end() && (it->day_cancel_deal_num > 3 || it->day_arb_fail_num >= 1))
      {
         res = false;
      }
      return res;
   }

   void add_arb_fail_num_once(name who, time_point_sec when, const std::string &reason)
   {
      userdaystat_index_t t(_self, who.value);
      auto it = t.find(day_start(when).sec_since_epoch());
      if (it == t.end())
      {
         t.emplace(_self, [&](userdaystat &u) {
            u.day_arb_fail_num = 1;
            u.source.append("|").append(reason);
         });
      }
      else
      {
         t.modify(it, _self, [&](userdaystat &u) {
            u.day_arb_fail_num++;
            u.source.append("|").append(reason);
         });
      }
   }

   void reduce_arb_fail_num_once(name who, time_point_sec when, const std::string &reason)
   {
      userdaystat_index_t t(_self, who.value);
      auto it = t.find(day_start(when).sec_since_epoch());
      if (it == t.end())
      {
         t.emplace(_self, [&](userdaystat &u) {
            u.day_arb_fail_num = 0;
            u.source.append("|").append(reason);
         });
      }
      else
      {
         t.modify(it, _self, [&](userdaystat &u) {
            u.day_arb_fail_num--;
            u.source.append("|").append(reason);
         });
      }
   }

   //用户表,scope 是一个个用户,用户名是小写
   TABLE exchangeuser
   {
      symbol_code stock; //代币，主键
      name account;      //EOS账户
      std::string nickname;
      asset balance;          //代币余额
      uint32_t ad_total_num;  //发布的广告数
      uint32_t ad_cancel_num; //下架的广告数

      uint32_t deal_total_num;          //交易总数
      uint32_t deal_nopay_num;          //没有法币支付的交易总数
      uint32_t deal_succcess_nun;       //交易成功（没有走仲裁和终审）
      uint32_t deal_arbiarate_win_num;  //仲裁获胜总数
      uint32_t deal_arbiarate_fail_num; //仲裁失败总数
      uint32_t deal_judge_win_num;      //终审成功总数
      uint32_t deal_judge_fail_num;     //终审失败总数
      asset taker_ask_fee_rate;
      asset taker_bid_fee_rate;
      asset maker_ask_fee_rate;
      asset maker_bid_fee_rate;

      uint64_t primary_key() const { return stock.raw(); }
   };
   using user_index_t = multi_index<"users"_n, exchangeuser>;

   //scope 是代币，主键是用户，保存一个代币下有哪些交易员
   TABLE userbystock
   {
      symbol_code stock; //
      name account;      //
      uint64_t primary_key() const { return account.value; }
   };
   using userbystock_index_t = multi_index<"userbystocks"_n, userbystock>;

   void inup_userbystock(const symbol_code &stock, name account)
   {
      std::string str_stock = stock.to_string();
      str_stock = lower_str(str_stock);
      userbystock_index_t t(_self, name{str_stock}.value);
      auto it = t.find(account.value);
      if (it == t.end())
      {
         t.emplace(_self, [&](userbystock &u) {
            u.stock = stock;
            u.account = account;
         });
      }
   }

   TABLE adnotfill
   {
      name user; //主键
      uint32_t num;
      uint64_t primary_key() const { return user.value; }
   };
   using adnotfill_index_t = multi_index<"adnotfills"_n, adnotfill>;

   void update_user_ad_unfill_num(name user, int32_t num)
   {
      adnotfill_index_t t(_self, _self.value);
      auto it = t.find(user.value);
      if (it == t.end())
      {
         t.emplace(_self, [&](adnotfill &i) {
            i.user = user;
            i.num = num;
         });
      }
      else
      {
         t.modify(it, _self, [&](adnotfill &i) {
            i.num = i.num + num;
         });
      }
   }

   void erase_adnotfills()
   {
      adnotfill_index_t t(_self, _self.value);
      auto it = t.begin();
      while (it != t.end())
      {
         it = t.erase(it);
      }
   }

   TABLE dealnotfill
   {
      name user; //主键
      uint64_t primary_key() const { return user.value; }
   };
   using dealnotfill_index_t = multi_index<"dealnotfills"_n, dealnotfill>;

   void erase_dealnotfills()
   {
      dealnotfill_index_t t(_self, _self.value);
      auto it = t.begin();
      while (it != t.end())
      {
         it = t.erase(it);
      }
   }

   bool check_user_deal_not_fill(name user)
   {
      dealnotfill_index_t t(_self, _self.value);
      auto it = t.find(user.value);
      return it != t.end();
   }

   void user_deal_not_fill(name user)
   {
      dealnotfill_index_t t(_self, _self.value);
      auto it = t.find(user.value);
      if (it == t.end())
      {
         t.emplace(_self, [&](dealnotfill &i) {
            i.user = user;
         });
      }
   }

   void user_deal_fill(name user)
   {
      dealnotfill_index_t t(_self, _self.value);
      auto it = t.find(user.value);
      if (it != t.end())
      {
         t.erase(it);
      }
   }

   //scope 是代币，主键是用户
   TABLE arbiter
   {
      symbol_code stock; //
      name account;      //EOS账户，仲裁者 主键
      std::string nickname;
      asset balance;       //质押还剩下的代币余额
      asset mortgage;      //总的质押总额
      asset total_revenue; //个人仲裁总收入
      asset today_predict_revenue;
      asset today_revenue; //今天个人的收入

      time_point_sec ctime;
      time_point_sec utime;
      time_point_sec last_arb_time; //最近一次仲裁时间
      uint32_t total_num;           //仲裁总场次，每次要更新
      uint32_t win_num;             //仲裁胜利场次
      uint32_t today_win_num;       //今天仲裁胜利场次
      uint8_t is_work;              //是否正在仲裁中
      uint32_t judge_animus_num;    //恶意仲裁数
      uint32_t judge_win_num;       //终审获胜数

      uint32_t online_beg_time; //在线开始时间[0,24] 精确到秒
      uint32_t online_end_time; //在线结束时间[0,24] 精确到秒
      uint8_t level;            //仲裁员配置信息
      uint8_t status;           //1.有效 2.invalid 无效
      std::string email;        //邮箱

      uint64_t primary_key() const { return account.value; }
      uint64_t get_work_status() const { return is_work; }
      uint64_t get_avail_time() const { return online_beg_time; }
      uint64_t get_win_num() const { return win_num; }
      double get_win_rate() const { return static_cast<double>(total_num == 0 ? 0.0 : (static_cast<double>(win_num) / total_num)); }
   };

   void update_arbiter_total_num(const symbol_code &stock, name who)
   {
      std::string str_stock = stock.to_string();
      str_stock = lower_str(str_stock);
      arbiter_index_t users_(_self, name{str_stock}.value);
      auto it = users_.require_find(who.value, "不是仲裁者");
      users_.modify(it, _self, [&](arbiter &a) {
         a.utime = CURRENT_SEC;
         a.total_num = a.total_num + 1;
         a.is_work = a.is_work + 1;
         a.last_arb_time = a.utime;
      });
   }

   void update_judger_total_num(const symbol_code &stock, name who)
   {
      std::string str_stock = stock.to_string();
      str_stock = lower_str(str_stock);
      judger_index_t users_(_self, name{str_stock}.value);
      auto it = users_.require_find(who.value, "不是仲裁者");
      users_.modify(it, _self, [&](judger &a) {
         a.utime = CURRENT_SEC;
         a.total_num = a.total_num + 1;
         a.is_work = a.is_work + 1;
         a.last_judge_time = a.utime;
      });
   }

   void update_arbiter_win_num(const symbol_code &stock, name who)
   {
      std::string str_stock = stock.to_string();
      str_stock = lower_str(str_stock);
      arbiter_index_t users_(_self, name{str_stock}.value);
      auto it = users_.require_find(who.value, "不是仲裁者");
      users_.modify(it, _self, [&](arbiter &a) {
         a.utime = CURRENT_SEC;
         a.win_num = a.win_num + 1;
      });
   }

   void rudece_arbiter_work_num(const symbol_code &stock, name who)
   {
      std::string str_stock = stock.to_string();
      str_stock = lower_str(str_stock);
      arbiter_index_t users_(_self, name{str_stock}.value);
      auto it = users_.require_find(who.value, "不是仲裁者");
      users_.modify(it, _self, [&](arbiter &a) {
         a.utime = CURRENT_SEC;
         a.is_work = a.is_work - 1;
      });
   }

   void rudece_judger_work_num(const symbol_code &stock, name who)
   {
      std::string str_stock = stock.to_string();
      str_stock = lower_str(str_stock);
      judger_index_t users_(_self, name{str_stock}.value);
      auto it = users_.require_find(who.value, "不是仲裁者");
      users_.modify(it, _self, [&](judger &a) {
         a.utime = CURRENT_SEC;
         a.is_work = a.is_work - 1;
      });
   }

   using arbiter_index_t = multi_index<"arbitrators"_n,
                                       arbiter,
                                       indexed_by<"byworkstatus"_n, const_mem_fun<arbiter, uint64_t, &arbiter::get_work_status>>,
                                       indexed_by<"bytime"_n, const_mem_fun<arbiter, uint64_t, &arbiter::get_avail_time>>,
                                       indexed_by<"bywinnum"_n, const_mem_fun<arbiter, uint64_t, &arbiter::get_win_num>>,
                                       indexed_by<"bywinrate"_n, const_mem_fun<arbiter, double, &arbiter::get_win_rate>>

                                       >;
   using arbiter_itr = arbiter_index_t::const_iterator;
   inline bool arbiter_isnotwork(const arbiter &er) { return er.is_work == 0; }
   inline bool arbiter_isenough(const arbiter &er, asset amount) { return er.balance >= amount; }

   //scope是用户,stock作为主键
   TABLE userrole
   {
      symbol_code stock; //主键
      name account;      //EOS账户
      uint8_t role;      //角色 1：交易用户 2.仲裁者(在线，正常) 3.终审者 4.交易用户+仲裁者 5.交易用户+终审员  6.仲裁者离线 7.仲裁者进小黑屋
      time_point_sec ctime;
      time_point_sec utime;
      uint64_t primary_key() const { return stock.raw(); }
   };

   using userrole_index_t = multi_index<"userroles"_n, userrole>;

   //scope是stock,主键是用户
   TABLE judger
   {
      symbol_code stock; //
      name account;      //EOS账户，仲裁者 主键
      std::string nickname;
      asset balance;  //质押还剩下的代币余额
      asset mortgage; //总的质押总额

      time_point_sec ctime;
      time_point_sec utime;
      time_point_sec last_judge_time; //最近一次仲裁时间
      uint32_t total_num;             //分配给我的总的终审订单数
      uint32_t cancelplay_num;        //取消放币
      uint32_t playcoin_num;          //终审放币胜利场次

      uint8_t is_work;          //是否正在仲裁中
      uint32_t online_beg_time; //在线开始时间[0,24] 精确到秒
      uint32_t online_end_time; //在线结束时间[0,24] 精确到秒
      uint8_t level;            //仲裁员配置信息
      uint8_t status;           //1.有效 2.invalid 无效
      std::string email;        //邮箱

      uint64_t primary_key() const { return account.value; }
      uint64_t get_work_status() const { return is_work; }
      uint64_t get_avail_time() const { return online_beg_time; }
   };
   inline bool judger_isnotwork(const judger &er) { return er.is_work == 0; }

   using judger_index_t = multi_index<"judgers"_n, judger,
                                      indexed_by<"byworkstatus"_n, const_mem_fun<judger, uint64_t, &judger::get_work_status>>,
                                      indexed_by<"bytime"_n, const_mem_fun<judger, uint64_t, &judger::get_avail_time>>>;

   /*scope是pair ,一个交易对里面的deal_id 是唯一的*/

   TABLE judge
   {
      uint64_t id;          //作为主键 =deal_id
      uint64_t deal_id;     //成交记录id
      uint64_t arborder_id; //仲裁订单id
      symbol_code pair;     //交易对
      time_point_sec ctime; //创建时间
      time_point_sec utime; //更新时间

      uint8_t judge_side; //终审者是买还是卖,只有失败方才能终审
      name bid_user;      //买币的用户
      name ask_user;      //卖币的用户
      name arb_failer;    //仲裁失败者,也就是终审发起方

      std::string content; //申诉的内容

      //phone email reason desc
      std::vector<std::string> contacts; //联系方式
      std::string reason;                //申诉原因
      std::string desc;                  //问题描述
      std::vector<std::string> imgs;
      std::vector<std::string> videos;
      std::string source; //备注
      std::string s1;     //备用
      std::string s2;     //备用
      uint64_t primary_key() const { return id; }
   };
   using judge_index_t = multi_index<"judges"_n, judge>;

   void erase_judges(const symbol_code &pair)
   {

      auto str_pair = pair.to_string();
      str_pair = lower_str(str_pair);
      judge_index_t t(_self, name{str_pair}.value);
      auto it = t.begin();
      while (it != t.end())
      {
         it = t.erase(it);
      }
      print("judges 表已经清理完成");
   }

   TABLE judgeorder
   {
      uint64_t id;          //仲裁id,实际==deal id
      time_point_sec ctime; //创建时间
      time_point_sec utime; //更新时间
      symbol_code pair;     //交易对
      uint64_t deal_id;
      uint64_t judge_id;      //申诉材料id
      uint8_t initiator_side; //发起者是买还是卖

      asset amount;                             //仲裁数量
      asset price;                              //仲裁价格
      asset quota;                              //终金额
      std::set<name> vec_judge;                 //仲裁员成员
      std::map<name, uint8_t> map_person_pick;  //每个人的仲裁结果
      std::map<name, uint64_t> map_person_task; //每个人的仲裁结果
      asset judge_fee;                          //仲裁手续费,完成这个arborder仲裁员的收入

      uint16_t person_num; //仲裁员人数
      uint16_t yes_num;    //同意放币
      uint16_t no_num;     //取消

      uint8_t status;         //终审进度:终审开始 终审中 终审结束
      std::string status_str; //仲裁进度，
      std::string source;     //备注信息

      uint64_t primary_key() const { return id; }
   };

   using judgeorder_index_t = multi_index<"judgeorders"_n, judgeorder>;
   using itr_judgeorder_t = judgeorder_index_t::const_iterator;

   void erase_judgeorders(const symbol_code &pair)
   {

      auto str_pair = pair.to_string();
      str_pair = lower_str(str_pair);
      judgeorder_index_t t(_self, name{str_pair}.value);
      auto it = t.begin();
      while (it != t.end())
      {
         it = t.erase(it);
      }
      print("judgeorders 表已经清理完成");
   }

   TABLE judgetask
   {
      uint64_t id; //自增主键
      uint8_t side;
      time_point_sec judge_time; //终审时间
      time_point_sec ctime;      //创建时间
      time_point_sec utime;      //更新时间

      symbol_code pair; //可以按pair过滤
      symbol_code stock;
      symbol_code money;
      uint64_t arborder_id;   //仲裁id
      uint64_t judgeorder_id; //仲裁id
      uint64_t deal_id;       //订单id
      uint64_t judge_id;      //交易双方申诉id

      asset amount;               //仲裁数量
      asset price;                //仲裁价格
      asset quota;                //交易金额
      uint8_t self_judge_result;  //我的仲裁结果
      uint8_t group_judge_result; //团仲裁结果 =0
      uint8_t group_yes_num;      //团内的仲裁放币人数
      uint8_t group_no_num;       //团内的取消放币人数

      asset labor_fee;        //劳务费，待结算状态为0,已结算状态下为正数，恶意仲裁为负数
      uint8_t status;         //1.仲裁收入 2.待结算 午夜12点结算 3.恶意仲裁 扣押金
      std::string status_str; //1.仲裁收入 2.待结算 午夜12点结算 3.恶意仲裁 扣押金
      std::string source;     //备注
      uint64_t primary_key() const { return id; }
      uint64_t by_status() const { return status; }
      uint64_t by_pair() const { return pair.raw(); }
      uint64_t by_stock() const { return stock.raw(); }
      uint64_t by_money() const { return money.raw(); }
   };
   using judgetask_index_t = multi_index<"judgetasks"_n, judgetask,
                                         indexed_by<"bystatus"_n, const_mem_fun<judgetask, uint64_t, &judgetask::by_status>>,
                                         indexed_by<"bypair"_n, const_mem_fun<judgetask, uint64_t, &judgetask::by_pair>>,
                                         indexed_by<"bystock"_n, const_mem_fun<judgetask, uint64_t, &judgetask::by_stock>>,
                                         indexed_by<"bymoney"_n, const_mem_fun<judgetask, uint64_t, &judgetask::by_money>>

                                         >;

   /*scope是pair ,一个交易对里面的deal_id 是唯一的*/

   void erase_judgetasks(name judgername)
   {

      judgetask_index_t d_(_self, judgername.value); //表名，按用户分表，不是按pair分表
      auto it = d_.begin();

      while (it != d_.end())
      {
         it = d_.erase(it);
      }
      print(judgername, "的终审任务表 clear ok");
   }

   TABLE xappeal
   {
      uint64_t id;            //作为主键 =deal_id
      uint64_t deal_id;       //成交记录id
      uint64_t arborder_id;   //仲裁订单id
      symbol_code pair;       //交易对
      uint8_t initiator_side; //发起者是买还是卖
      time_point_sec ctime;   //创建时间
      time_point_sec utime;   //更新时间
      name bid_user;          //买币的用户
      name ask_user;          //卖币的用户

      std::string bid_content; //买币放的申诉的内容
      std::string ask_content; //买币放的申诉的内容

      //phone email reason desc
      std::vector<std::string> ask_contacts;
      std::string ask_reason;
      std::string ask_desc;
      std::string ask_s1; //备用
      std::string ask_s2; //备用

      std::vector<std::string> bid_contacts;
      std::string bid_reason;
      std::string bid_desc;
      std::string bid_s1; //备用
      std::string bid_s2; //备用

      std::vector<std::string> ask_imgs;
      std::vector<std::string> bid_imgs;
      std::vector<std::string> bid_videos;
      std::vector<std::string> ask_videos;
      std::string source;
      uint64_t primary_key() const { return id; }
   };

   using xappeal_index_t = multi_index<"xappeals"_n, xappeal>;

   void erase_appeals(const symbol_code &pair)
   {
      auto str_pair = pair.to_string();
      str_pair = lower_str(str_pair);
      xappeal_index_t appeals(_self, name{str_pair}.value);
      auto it = appeals.begin();
      while (it != appeals.end())
      {
         it = appeals.erase(it);
      }
      print("appeals 表已经清理完成");
   }

   /*scope是pair ,一个交易对里面的deal_id 是唯一的*/
   TABLE arborder
   {
      uint64_t id;          //仲裁id,实际==deal id
      time_point_sec ctime; //创建时间
      time_point_sec utime; //更新时间
      symbol_code pair;     //交易对
      uint64_t deal_id;
      uint64_t appeal_id;     //申诉材料id
      uint8_t initiator_side; //发起者是买还是卖

      asset amount;                             //仲裁数量
      asset price;                              //仲裁价格
      asset quota;                              //终金额
      std::set<name> vec_arbiter;               //仲裁员成员
      std::map<name, uint8_t> map_person_pick;  //每个人的仲裁结果
      std::map<name, uint64_t> map_person_task; //每个人的仲裁订单
      asset arbitrate_fee;                      //仲裁手续费,完成这个arborder仲裁员的收入

      uint16_t person_num;    //仲裁员人数
      uint16_t yes_num;       //同意放币
      uint16_t no_num;        //取消
      name judger;            //终审者
      uint8_t judge_result;   //终审结果
      bool has_judge_arbcmd;  //仲裁结果是否被执行
      uint8_t status;         //仲裁进度:仲裁开始，仲裁中，仲裁结束， 终审开始 终审中 终审结束
      std::string status_str; //仲裁进度，
      std::string source;     //备注信息

      uint64_t primary_key() const { return id; }
   };

   using arborder_index_t = multi_index<"arborders"_n, arborder>;
   using itr_arborder_t = arborder_index_t::const_iterator;

   void erase_arborders(const symbol_code &pair)
   {
      require_auth(_self);
      auto str_pair = pair.to_string();
      str_pair = lower_str(str_pair);
      arborder_index_t t(_self, name{str_pair}.value);
      auto it = t.begin();
      while (it != t.end())
      {
         it = t.erase(it);
      }
      print("arborders 表已经清理完成");
   }

   ///每个人的仲裁记录就是deal了,

   TABLE arbtask
   {
      uint64_t id; //自增主键
      uint8_t side;
      time_point_sec appeal_time; //申诉时间
      time_point_sec ctime;       //创建时间
      time_point_sec utime;       //更新时间

      symbol_code pair; //可以按pair过滤
      symbol_code stock;
      symbol_code money;
      uint64_t arborder_id; //仲裁id
      uint64_t deal_id;     //订单id
      uint64_t appeal_id;   //交易双方申诉id

      asset amount;               //仲裁数量
      asset price;                //仲裁价格
      asset quota;                //交易金额
      uint8_t self_arbit_result;  //我的仲裁结果
      uint8_t group_arbit_result; //团仲裁结果 =0
      uint8_t group_yes_num;      //团内的仲裁放币人数
      uint8_t group_no_num;       //团内的取消放币人数

      asset labor_fee;        //劳务费，待结算状态为0,已结算状态下为正数，恶意仲裁为负数
      uint8_t status;         //1.仲裁收入 2.待结算 午夜12点结算 3.恶意仲裁 扣押金
      std::string status_str; //1.仲裁收入 2.待结算 午夜12点结算 3.恶意仲裁 扣押金
      std::string source;     //备注
      uint64_t primary_key() const { return id; }
      uint64_t by_status() const { return status; }
      uint64_t by_pair() const { return pair.raw(); }
      uint64_t by_stock() const { return stock.raw(); }
      uint64_t by_money() const { return money.raw(); }
      uint64_t by_ctime() const { return ctime.sec_since_epoch(); }
   };
   using arbtask_index_t = multi_index<"arbtasks"_n, arbtask,
                                       indexed_by<"bystatus"_n, const_mem_fun<arbtask, uint64_t, &arbtask::by_status>>,
                                       indexed_by<"bypair"_n, const_mem_fun<arbtask, uint64_t, &arbtask::by_pair>>,
                                       indexed_by<"bystock"_n, const_mem_fun<arbtask, uint64_t, &arbtask::by_stock>>,
                                       indexed_by<"bymoney"_n, const_mem_fun<arbtask, uint64_t, &arbtask::by_money>>,
                                       indexed_by<"byctime"_n, const_mem_fun<arbtask, uint64_t, &arbtask::by_ctime>>

                                       >;

   void erase_arbtasks(name arbitername)
   {

      arbtask_index_t d_(_self, arbitername.value); //表名，按用户分表，不是按pair分表
      auto it = d_.begin();

      while (it != d_.end())
      {
         it = d_.erase(it);
      }
      print(arbitername, "的仲裁任务表 clear ok");
   }

   TABLE market
   {
      symbol_code pair;           //交易对
      symbol stock;               //coin 需要指定[代币精度,代币符号]
      symbol money;               //fiat 需要指定[法币精度,法币符合]
      int64_t taker_ask_fee_rate; // taker手续费费率[double×pow(10,4)]
      int64_t taker_bid_fee_rate; // taker手续费费率[double×pow(10,4)]
      int64_t maker_ask_fee_rate; // maker手续费费率[double×pow(10,4)]
      int64_t maker_bid_fee_rate; // maker手续费费率[double×pow(10,4)]

      asset amount_min;                  // 限定的最小交易数量  [数量，代币精度，代币符号]
      asset amount_max;                  // 限定的最大交易数量  [数量，代币精度，代币符号]
      asset price_min;                   // 限定的最小交易价格  [数量，代币精度，代币符号]
      asset price_max;                   // 限定的最大交易价格  [数量，代币精度，代币符号]
      asset zero_stock;                  //代币的0值
      asset zero_money;                  //法币的0值
      asset zero_rate;                   //费率的0值
      uint32_t pay_timeout;              //买币方支付法币的超时时间,精确到s
      uint32_t self_playcoin_time;       //自己放币延迟时间
      uint32_t def_cancel_time;          //初次仲裁取消，延迟取消的时间,精确到s,这段时间用户可以发起终审
      uint32_t def_playcoin_time;        //初次仲裁通过，延迟放币的时间,精确到s，这段时间用户可以发起终审
      uint32_t cancel_ad_num;            //允许用户取消广告单次数
      uint8_t status = MARKET_STATUS_ON; //交易对是否允许交易
      std::string str_status;
      std::string nickname; //昵称
      std::string stockname;
      std::string moneyname;
      time_point_sec ctime{current_time_point().sec_since_epoch()};
      time_point_sec utime{current_time_point().sec_since_epoch()};

      uint64_t primary_key() const { return pair.raw(); }
      uint64_t get_secondary_status() const { return status; }     // sort by status，按交易对状态过滤
      uint64_t get_secondary_stock() const { return stock.raw(); } // sort by status，按交易对状态过滤
      uint64_t get_secondary_fiat() const { return money.raw(); }  // sort by status，按交易对状态过滤
   };

   using market_index_t = multi_index<"markets"_n,
                                      market,
                                      indexed_by<"bystatus"_n, const_mem_fun<market, uint64_t, &market::get_secondary_status>>,
                                      indexed_by<"bystock"_n, const_mem_fun<market, uint64_t, &market::get_secondary_stock>>,
                                      indexed_by<"byfiat"_n, const_mem_fun<market, uint64_t, &market::get_secondary_fiat>>>;

   using market_iter_t = market_index_t::const_iterator;

   market_index_t markets_; //属于合约，scope是全表

   //加一个全局概括
   TABLE overview
   {
      std::set<std::string> stocks;
      std::set<std::string> fiats;
      std::set<std::string> pairs;
      std::map<std::string, uint8_t> precisions;
      EOSLIB_SERIALIZE(overview, (stocks)(fiats)(pairs)(precisions))
   };
   using overview_t = singleton<"ow"_n, overview>;
   using overview_tt = multi_index<"ow"_n, overview>; // This next typedef is only here because of this bug: https://github.com/EOSIO/eosio.cdt/issues/280// Once that's fixed this can be removed.
   overview_t overview_;

   TABLE adorder
   {
      uint64_t id;            //广告订单id，自增主键,scope = pair+side.相当于盘口,卖盘，由小到大，买盘由大到小
      name user;              //广告订单所属用户，注意是name类型
      symbol_code pair;       //广告订单的交易对
      time_point ctime;       //广告订单创建时间，精确到微秒
      time_point utime;       //广告订单更新时间，精确到微秒
      uint8_t status;         //广告订单状态，吃单和挂单的状态不同
      std::string status_str; //广告订单状态，吃单和挂单的状态不同
      std::string nickname;
      uint8_t side;               //买卖类型，1卖 2买
      uint8_t type;               //广告订单类型，1限价 2市价
      uint8_t role;               //广告订单类型，1挂单 2吃单
      asset price;                //广告订单交易价格
      asset amount;               //广告订单交易数量
      asset amount_min;           //广告订单最小成交数量
      asset amount_max;           //广告订单最大成交数量
      int64_t maker_ask_fee_rate; //吃单的手续费率(只展示使用)
      int64_t maker_bid_fee_rate; //挂单的手续费率（只展示使用）

      asset left;                                    //剩余多少数量未成交
      asset freeze;                                  //冻结的stock或者money,处于正在交易中,只有卖币挂单才有值
      asset deal_fee;                                //累计的交易手续费
      asset deal_stock;                              //累计的交易sotck数量
      asset deal_money;                              //累计的交易money
      std::string source;                            //备注信息，广告订单来源
      std::vector<uint64_t> vec_deal;                //成交明细，因为一个广告订单可以被很多人来交易
      std::vector<uint64_t> pay_accounts;            //支持的法币帐号
      std::vector<uint64_t> pay_ways;                //支持的法币帐号
      std::vector<std::string> pay_account_contexts; //支持的法币帐号

      uint64_t primary_key() const { return id; }                   // primary key auto increase
      uint64_t get_secondary_user() const { return user.value; }    // sort by user name，按用户名过滤
      uint64_t get_secondary_price() const { return price.amount; } // sort by price, ask increase bid of deincrease
      uint64_t get_secondary_status() const { return status; }      // sort by price, ask increase bid of deincrease
      uint8_t update_ad_status(market_iter_t itr_pair)
      {
         //每次广告可交易数量变动后,对广告进行判断
         if (status == AD_STATUS_ONTHESHELF)
         { //广告为上架中

            if (left < itr_pair->amount_min)
            { //变动后交易数量小于交易对的最小交易数量时,状态改为自动下架
               status = AD_STATUS_AUT_OFFTHESHELF;
               status_str = AD_STATUS_AUT_OFFTHESHELF_STR;
            }
         }
         else if (status == AD_STATUS_AUT_OFFTHESHELF) //如果是自动下架,比如吃单方撤单了
         {
            if (left >= itr_pair->amount_min) //自动下架且可交易数量>=交易对配置的最小交易数量时,状态改为上架中
            {
               status = AD_STATUS_ONTHESHELF;         //改为上架中
               status_str = AD_STATUS_ONTHESHELF_STR; //改为上架中
            }
            else if (left + freeze < itr_pair->amount_min) //自动下架且可交易数量+冻结数量<交易对最小交易数量时，状态改为已完成
            {
               status = AD_STATUS_FINISHED;         //已完成
               status_str = AD_STATUS_FINISHED_STR; //已完成
            }
         }

         return status;
      }
   };

   using adorder_index_t = multi_index<"adorders"_n,
                                       adorder,
                                       indexed_by<"byuser"_n, const_mem_fun<adorder, uint64_t, &adorder::get_secondary_user>>,
                                       indexed_by<"byprice"_n, const_mem_fun<adorder, uint64_t, &adorder::get_secondary_price>>,
                                       indexed_by<"bystatus"_n, const_mem_fun<adorder, uint64_t, &adorder::get_secondary_status>>

                                       >;

   using adorder_iter_t = adorder_index_t::const_iterator;

   //scope 是用户名
   TABLE deallog
   {
      uint64_t id;      //自增id
      uint64_t deal_id; //成交记录
      uint64_t ad_id;   //广告id
      symbol_code pair; //交易对
      symbol_code stock;
      symbol_code money;
      uint8_t side;
      uint8_t role;
      asset price;
      asset amount;
      asset quota;
      uint8_t status;
      time_point ctime;
      time_point utime;
      uint64_t primary_key() const { return id; }
      uint64_t by_pair() const { return pair.raw(); }
      uint64_t by_status() const { return status; }
      uint64_t by_stock() const { return stock.raw(); }
      uint64_t by_money() const { return money.raw(); }
      uint64_t by_side() const { return side; }
   };

   using deallog_index_t = multi_index<"dealhistory"_n,
                                       deallog,
                                       indexed_by<"bypair"_n, const_mem_fun<deallog, uint64_t, &deallog::by_pair>>,
                                       indexed_by<"bystatus"_n, const_mem_fun<deallog, uint64_t, &deallog::by_status>>,
                                       indexed_by<"bystock"_n, const_mem_fun<deallog, uint64_t, &deallog::by_stock>>,
                                       indexed_by<"bymoney"_n, const_mem_fun<deallog, uint64_t, &deallog::by_money>>,
                                       indexed_by<"byside"_n, const_mem_fun<deallog, uint64_t, &deallog::by_side>>

                                       >;

   void erase_dhs(name who)
   {
      deallog_index_t dh_(_self, who.value);
      auto it = dh_.begin();
      while (it != dh_.end())
      {
         it = dh_.erase(it);
      }
   }

   //此表属于合约,scope是交易对,
   TABLE deal
   {
      uint64_t id;       //deal id  自增id,taker的交易id
      uint64_t taker_id; //deal id  自增id
      uint64_t maker_id; //deal id  自增id

      name user;
      uint8_t side;            //taker是买还是卖
      uint8_t type;            //广告订单类型，1限价 2市价
      uint8_t role;            //广告订单类型，1挂单 2吃单
      uint8_t appeal_side;     //申诉发起方
      time_point ctime;        //创建时间
      time_point utime;        //更新时间
      name maker_user;         //对手方
      uint64_t maker_order_id; //对手方广告订单id
      std::string tk_nickname;
      std::string mk_nickname;
      asset price;                                       //成交价
      asset amount;                                      //成交数量
      asset quota;                                       //成交额price*amount
      asset ask_fee;                                     //收取的卖方手续费
      asset bid_fee;                                     //收取的买方手续费
      uint8_t status;                                    //成交的状态
      std::string status_str;                            //成交的状态字符串
      uint32_t pay_timeout;                              //支付超时时间,
      uint64_t pay_timeout_sender_id;                    //异步事物id
      uint64_t self_playcoin_sender_id;                  //异步事物id
      name arb_failer;                                   //仲裁失败方
      name judge_failer;                                 //终审失败方
      uint64_t arb_sender_id;                            //仲裁异步事件
      uint32_t def_cancel_time;                          //初次仲裁取消，延迟取消的时间,精确到s,这段时间用户可以发起终审
      uint32_t def_playcoin_time;                        //初次仲裁通过，延迟放币的时间,精确到s，这段时间用户可以发起终审
      std::vector<uint64_t> ask_pay_accounts;            //支付方式或者支付id列表
      std::vector<uint64_t> bid_pay_accounts;            //支付方式或者支付id列表
      std::vector<uint64_t> ask_pay_ways;                //支持的法币帐号
      std::vector<uint64_t> bid_pay_ways;                //支持的法币帐号
      std::vector<std::string> ask_pay_account_contexts; //支持的法币帐号
      std::vector<std::string> bid_pay_account_contexts; //支持的法币帐号
      symbol_code pair;                                  //交易对
      uint64_t fiat_pay_method;                          //法币支付方式
      std::string fiat_account;                          //法币帐号,保存的是加密后的信息
      std::string source;                                //备注信息，仲裁信息等

      uint64_t primary_key() const { return id; }
      uint64_t get_secondary_user() const { return user.value; }
      uint64_t get_secondary_maker_user() const { return maker_user.value; }

      uint64_t get_status() const { return status; }
   };

   uint64_t new_dh_history(name who, uint64_t deal_id, uint64_t ad_id, const symbol_code &pair, uint8_t side, uint8_t role, const asset &price, const asset &amount, const asset &quota, time_point_sec ctime, time_point_sec utime)
   {
      deallog_index_t dh_(_self, who.value);
      auto ret = dh_.available_primary_key();
      auto it_pair = get_market(pair);
      dh_.emplace(_self, [&](deallog &d) {
         d.id = ret;
         d.deal_id = deal_id;
         d.ad_id = ad_id;
         d.pair = pair;
         d.stock = it_pair->stock.code();
         d.money = it_pair->money.code();
         d.side = side;
         d.role = role;
         d.price = price;
         d.amount = amount;
         d.quota = quota;
         d.status = DEAL_STATUS_UNPAID;
         d.ctime = ctime;
         d.utime = utime;
      });
      return ret;
   }
   void update_dh_status(name who, uint64_t pk_id, uint8_t status)
   {
      deallog_index_t dh_(_self, who.value);
      auto it = dh_.require_find(pk_id, "deal history not found");
      dh_.modify(it, _self, [&](deallog &d) {
         d.status = status;
         d.utime = CURRENT_SEC;
      });
   }

   using deal_index_t = multi_index<"deals"_n,
                                    deal,
                                    indexed_by<"byuser"_n, const_mem_fun<deal, uint64_t, &deal::get_secondary_user>>,
                                    indexed_by<"bystatus"_n, const_mem_fun<deal, uint64_t, &deal::get_status>>

                                    >;

   using deal_iter_t = deal_index_t::const_iterator;

private:
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
   inline static uint8_t update_ad_status(adorder & ad, market_iter_t itr_pair)
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

   inline adorder_index_t get_adtable(const symbol_code &pair, const std::string &side)
   {
      check((side == MARKET_ORDER_SIDE_ASK_STR || side == MARKET_ORDER_SIDE_BID_STR), SIDE_INVALID_STR);
      std::string scope = pair.to_string() + MARKET_ROLE_MAKER_STR + side;
      transform(scope.begin(), scope.end(), scope.begin(), ::tolower); //转成小写
      return adorder_index_t(_self, name{scope}.value);
   }

   inline adorder_iter_t get_adorder(const symbol_code &pair, const std::string &side, uint64_t ad_id)
   {

      auto adtable_ = get_adtable(pair, side);
      auto it = adtable_.require_find(ad_id, ERR_MSG_AD_NOT_EXIST);
      return it;
   }

   inline std::string lower_str(const std::string &src)
   {
      std::string res{src};
      std::transform(res.begin(), res.end(), res.begin(), ::tolower);
      return res;
   }
   inline std::string upper_str(const std::string &src)
   {
      std::string res{src};
      std::transform(res.begin(), res.end(), res.begin(), ::toupper);
      return res;
   }

   inline market_iter_t get_open_market(const symbol_code &pair) //
   {
      auto it = markets_.require_find(pair.raw(), ERR_MSG_CHECK_FAILED(50110, pair.to_string().append(ERR_MSG_PAIR_NOT_EXIST)).c_str());
      check(it->status == MARKET_STATUS_ON, ERR_MSG_CHECK_FAILED(50111, pair.to_string().append(ERR_MSG_PAIR_NOT_ALLOW_TRADE)));
      return it;
   }

   inline market_iter_t get_close_market(const symbol_code &pair) //
   {
      auto it = markets_.require_find(pair.raw(), ERR_MSG_CHECK_FAILED(50110, pair.to_string().append(ERR_MSG_PAIR_NOT_EXIST)).c_str());
      check(it->status == MARKET_STATUS_OFF, ERR_MSG_CHECK_FAILED(50111, pair.to_string().append(ERR_MSG_PAIR_NOT_ALLOW_TRADE)));
      return it;
   }

   inline market_iter_t get_market(const symbol_code &pair) //
   {
      auto it = markets_.require_find(pair.raw(), ERR_MSG_CHECK_FAILED(50110, pair.to_string().append(ERR_MSG_PAIR_NOT_EXIST)).c_str());
      return it;
   }
   inline void erase_markets()
   {
      auto it = markets_.begin();
      while (it != markets_.end())
      {
         it = markets_.erase(it);
      }
   }

   inline void erase_arbsts()
   {
      auto it = arbsts_.begin();
      while (it != arbsts_.end())
      {
         it = arbsts_.erase(it);
      }
   }

   void erase_deals(const symbol_code &pair)
   {
      auto str_pair = pair.to_string();
      str_pair = lower_str(str_pair);
      deal_index_t dealtable_(_self, name{str_pair}.value);
      auto it = dealtable_.begin();

      while (it != dealtable_.end())
      {
         it = dealtable_.erase(it);
      }
   }

   inline void erase_adorders(const symbol_code &pair, const std::string &side)
   {
      auto adtable_ = get_adtable(pair, side);
      auto it = adtable_.begin();
      while (it != adtable_.end())
      {
         it = adtable_.erase(it);
      }
   }

   inline std::string get_deal_status_str(uint8_t status)
   {
      switch (status)
      {
      case DEAL_STATUS_UNPAID_MAN_CANCEL:
         return DEAL_STATUS_UNPAID_MAN_CANCEL_STR;

      case DEAL_STATUS_UNPAID_TIMEOUT_CANCEL:
         return DEAL_STATUS_UNPAID_TIMEOUT_CANCEL_STR;

      case DEAL_STATUS_SUCCESS_FINISHED:
         return DEAL_STATUS_SUCCESS_FINISHED_STR;

      case DEAL_STATUS_ARB_CANCEL_FINISHED:
         return DEAL_STATUS_ARB_CANCEL_FINISHED_STR;

      case DEAL_STATUS_ARB_PLAYCOIN_FINISHED:
         return DEAL_STATUS_ARB_PLAYCOIN_FINISHED_STR;

      case DEAL_STATUS_JUD_CANCEL_FINISHED:
         return DEAL_STATUS_JUD_CANCEL_FINISHED_STR;

      case DEAL_STATUS_JUD_PLAYCOIN_FINISHED:
         return DEAL_STATUS_JUD_PLAYCOIN_FINISHED_STR;

      case DEAL_STATUS_PAID_WAIT_PLAYCOIN:
         return DEAL_STATUS_PAID_WAIT_PLAYCOIN_STR;

      case DEAL_STATUS_PAID_PLAYCOIN_ING:
         return DEAL_STATUS_PAID_PLAYCOIN_ING_STR;

      case DEAL_STATUS_PAID_APPEAL_ASK:
         return DEAL_STATUS_PAID_APPEAL_ASK_STR;
      case DEAL_STATUS_PAID_APPEAL_BID:
         return DEAL_STATUS_PAID_APPEAL_BID_STR;
      case DEAL_STATUS_PAID_APPEAL_ALL:
         return DEAL_STATUS_PAID_APPEAL_ALL_STR;

      case DEAL_STATUS_PAID_ARBIARATE_ING:
         return DEAL_STATUS_PAID_ARBIARATE_ING_STR;
      case DEAL_STATUS_PAID_ARBIARATE_CANCEL:
         return DEAL_STATUS_PAID_ARBIARATE_CANCEL_STR;
      case DEAL_STATUS_PAID_ARBIARATE_PALYCOIN:
         return DEAL_STATUS_PAID_ARBIARATE_PALYCOIN_STR;

      case DEAL_STATUS_PAID_JUDGING:
         return DEAL_STATUS_PAID_JUDGING_STR;
      }

      return "";
   }
   inline bool is_today(uint32_t last_time)
   {
      return current_time_point().sec_since_epoch() / 86400 == last_time / 86400;
   }
   inline time_point_sec day_start(time_point_sec when)
   {
      return time_point_sec(when.sec_since_epoch() - when.sec_since_epoch() % DAY_SECONDS);
   }

   void log(const std::string &msg)
   {
      log_.emplace(_self, [&](logitem &item) {
         item.id = log_.available_primary_key();
         item.ctime = CURRENT_SEC;
         item.msg = msg;
      });
   }

   bool cancel_deftransaction(const uint64_t &sender_id, const std::string &reason)
   {
      if (sender_id == 0)
         return true;
      auto res = cancel_deferred(sender_id);
      print(res, "(tip:1 if transaction was canceled, 0 if transaction was not found) ", reason.c_str());
      log(reason);
      return res == 1;
   }

   uint8_t get_level(const symbol_code &stock, asset balance)
   {
      auto state = overview_.get();
      auto it_precision = state.precisions.find(stock.to_string());
      check(it_precision != state.precisions.end(), "代币不存在，请先创建代币的交易对");
      asset zero = asset(0, symbol(stock, it_precision->second));
      check(balance > zero, "抵押代币必须大于0");
      auto it = arbsts_.require_find(stock.raw(), "找不到仲裁等级配置项");
      uint8_t level = 0;
      for (auto &item : it->vec_level)
      {
         if (balance >= item)
         {
            ++level;
            continue;
         }
         else
         {
            break;
         }
      }
      return level;
   }

   void get_avail_arbiter(const symbol_code &stock, std::set<name> &res, int num, const time_point_sec &ctime, deal_iter_t itr_deal);
   void get_avail_judger(const symbol_code &stock, std::set<name> &res, int num, const time_point_sec &ctime, deal_iter_t itr_deal);

   int update_art(name arbiter, const symbol_code &pair, uint64_t deal_id, uint8_t choice, const std::string &reason);

   void rollback_deal(deal_index_t & dealtable_,
                      deal_iter_t itr_deal,
                      uint8_t status,
                      const std::string &reason);

   void commit_deal(name asker,
                    deal_index_t & dealtable_,
                    deal_iter_t itr_deal,
                    uint8_t status,
                    const std::string &reason);

   bool arb_can_cmd(const symbol_code &pair, uint64_t deal_id, const std::string &reason);

   inline int64_t double_to_pow4(double num)
   {
      static auto pr4 = std::pow(10, 4);
      return static_cast<int64_t>(num * pr4 + 0.5);
   }

   //std::pair<asset, asset> get_user_fee_rate(name user, const symbol_code &pair, uint8_t role);
   std::pair<int64_t, int64_t> get_user_fee_rate_(name user, const symbol_code &pair, uint8_t role);
   std::string get_user_nickname(name user);
   std::string get_user_pay_accout_content(name asker, uint64_t pmt_id);

   asset get_fee(name user, const symbol_code &pair, asset amount, uint8_t askorbid, uint8_t role);
   asset cal_asset( asset amount, uint32_t ratio);
   inline asset cal_fee(asset amount, int64_t fee_rate)
   {
      static auto pr4 = std::pow(10, 4);
      return asset(static_cast<int64_t>(amount.amount * (fee_rate) / pr4 + 0.5), amount.symbol);
   }

   bool isnotarber(name user, const symbol_code &stock);

   bool isnotjudger(name user, const symbol_code &stock);

   uint8_t get_userrole(name user, const symbol_code &stock);

   void delete_userrole(name user, const symbol_code &stock, uint8_t role);

   void insert_modify_userrole(name user, const symbol_code &stock, uint8_t role);

   template <typename T>
   void clear_tablebystock(const symbol_code &stock)
   {
      T t(_self, stock.raw());
      auto it = t.begin();
      while (it != t.end())
      {
         it = t.erase(it);
      }
   }

   template <typename T>
   void clear_tablebypair(const symbol_code &pair)
   {
      auto str_pair = pair.to_string();
      str_pair = lower_str(str_pair);
      T t(_self, name{str_pair}.value);
      auto it = t.begin();
      while (it != t.end())
      {
         it = t.erase(it);
      }
   }

   template <typename T>
   void clear_tablebyname(name who)
   {

      T t(_self, who.value);
      auto it = t.begin();
      while (it != t.end())
      {
         it = t.erase(it);
      }
   }

   inline void def_trans_addblacklist(name cmder, name failer, deal_iter_t itr_deal, const std::string &reason)
   {
      transaction t{};
      t.actions.emplace_back(
          permission_level(cmder, name{"active"}),
          name{PRO_TOKEN_CONTRACT_NAME},
          name{"addblacklist"},
          std::make_tuple(failer));
      

      t.delay_sec = DAY_SECONDS * 2; //延迟48h执行，48h发起二次终审的话，还要取消这个延长事务
      
      t.send(itr_deal->pay_timeout_sender_id, _self);
   }

public:
   inline void checkinblacklist(name who)
   {
   }
   inline void transfer(name from,
                        name to,
                        asset quantity,
                        const std::string &memo)
   {
      //可以冻结,转账,扣手续费等
      action(
          permission_level{from, name{"active"}},
          name{TOKEN_CONTRACT_NAME},
          name{TOKEN_CONTRACT_TRANSFER_ACTION},
          std::make_tuple(from, to, quantity, memo))
          .send();
   }

   // cc push push action otcsystem addblacklist '["eoscngdszlgz"]' -p otcsystem

   /* owner: 存款人账号。
   * assset: 资产数额。
   * type: 1 OTC 2 ABT(仲裁）
   * cc push action otcsystem deposit '[ "eoscngdszlgz", "100.0000 ADX", 1]' -p eoscngdszlgz
   */

   inline void freeze_stock(name from,
                            name to,
                            asset quantity,
                            uint8_t type,
                            const std::string &memo)
   {
#ifdef ENV_DEV // 自己测试时,按转账的逻辑做
      action(
          permission_level{from, name{"active"}},
          name{TOKEN_CONTRACT_NAME},
          name{TOKEN_CONTRACT_TRANSFER_ACTION},
          std::make_tuple(from, to, quantity, memo))
          .send();

#else //联调时 调其他合约
      action{
          permission_level{from, name{"active"}},
          name{PRO_TOKEN_CONTRACT_NAME},
          name{PRO_TOKEN_CONTRACT_FREEZE},
          std::make_tuple(from, quantity, type)}
          .send();
#endif
   }

   //撤单解冻
   /* owner: 存款人账号。
   * assset: 资产数额。
   * type: 1 OTC 2 ABT(仲裁）
   * cc push action otcsystem withdraw '[ "eoscngdszlgz", "10.0000 ADX", 1]' -p 
   */
   inline void unfreeze_stock(name from,
                              name to,
                              asset quantity,
                              uint8_t type,
                              const std::string &memo)
   {

#ifdef ENV_DEV
      action(
          permission_level{from, name{"active"}},
          name{TOKEN_CONTRACT_NAME},
          name{TOKEN_CONTRACT_TRANSFER_ACTION},
          std::make_tuple(from, to, quantity, memo))
          .send();

#else
      action(
          permission_level{to, name{"active"}},
          name{PRO_TOKEN_CONTRACT_NAME},
          name{PRO_TOKEN_CONTRACT_UNFREEZE},
          std::make_tuple(to, quantity, type))
          .send();
#endif
   }
   /*
   * owner: 存款人账号。
     to: 目标账号。
     assset: 资产数额。
     fee: 手续费数额
     askorbid:谁出手续费 true：买方 fasle卖方  
   *  cc push action otcsystem transfer '["eoscngdszlgz", "liguozheng11", "10.0000 ADX", "1.0000 ADX"]' -p eoscngdszlgz  
   */
   inline void settle_stock(name from,
                            name to,
                            asset quantity,
                            asset fee,
                            const std::string &memo,
                            bool askorbid)
   {
#ifdef ENV_DEV
      action(
          permission_level{from, name{"active"}},
          name{TOKEN_CONTRACT_NAME},
          name{TOKEN_CONTRACT_TRANSFER_ACTION},
          std::make_tuple(from, to, quantity, memo))
          .send();

#else
      //可以冻结,转账,扣手续费等
      action(
          permission_level{from, name{"active"}},
          name{PRO_TOKEN_CONTRACT_NAME},
          name{PRO_TOKEN_CONTRACT_SETTLE},
          std::make_tuple(from, to, quantity, fee, askorbid))
          .send();
#endif
   }

   //on_notify函数不是可供直接调用的合约函数，它是一个通知处理程序（notification handler）必须指明监听的合约名（code）和Action名（action），这样可以避免处理从非eosio.token合约发送来的“假EOS”，提高了安全性
   //on_notify函数能够监听eosio.token::transfer，是因为eosio.token::transfer内部调用了require_recipient(to)函数
   //on_notify属性接受通配符，比如这样定义：[[eosio::on_notify("*::transfer")]] 可以让on_notify函数处理任何合约的transfer Action。
   [[eosio::on_notify("eosio.token::transfer")]] void onTransfer(name from, name to, asset quantity, std::string memo)
   {
      /*
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
      */
   }

   //如果延期交易执行失败，EOSIO会发送一个eosio::onerror的Action，可以使用on_notify监听此Action，从onerror对象获取sender_id和打包后的transaction数据，进行错误处理
   [[eosio::on_notify("eosio::onerror")]] void onError(const onerror &error)
   {

      print("Resending Transaction: ", error.sender_id);
      log_.emplace(_self, [&](logitem &item) {
         item.id = log_.available_primary_key();
         item.ctime = CURRENT_SEC;
         std::string s{"def transaction fail:"};
         s.append(std::to_string(uint64_t(error.sender_id)));
         item.msg = s;
      });

      transaction dtrx = error.unpack_sent_trx();
      dtrx.delay_sec = 3;

      //dtrx.send(eosio::current_time_point().sec_since_epoch(), _self);
   }
};

namespace eosiosystem
{
   struct [[eosio::table, eosio::contract("eosio.system")]] user_resources
   {
      name owner;
      asset net_weight;
      asset cpu_weight;
      int64_t ram_bytes = 0;

      bool is_empty() const { return net_weight.amount == 0 && cpu_weight.amount == 0 && ram_bytes == 0; }
      uint64_t primary_key() const { return owner.value; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE(user_resources, (owner)(net_weight)(cpu_weight)(ram_bytes))
   };
   typedef eosio::multi_index<"userres"_n, user_resources> user_resources_table;

} // namespace eosiosystem
