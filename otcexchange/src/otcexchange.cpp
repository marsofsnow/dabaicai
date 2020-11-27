#include "otcexchange.hpp"
#include "otcsystem.hpp"
#include <eosio/print.hpp>
#include <algorithm>

//#include "nlohmann/json.hpp"
//using namespace nlohmann;

ACTION otcexchange::hi(name nm)
{
   print_f("current version:", VERSION);
}

ACTION otcexchange::cleartables()
{
   //清理scope是合约
   erase_adnotfills();
   erase_dealnotfills();
   //先清交易对相关的表
   {
      auto beg = markets_.begin();
      while (beg != markets_.end())
      {
         //广告表
         erase_adorders(beg->pair, MARKET_ORDER_SIDE_ASK_STR);
         erase_adorders(beg->pair, MARKET_ORDER_SIDE_BID_STR);
         //deal表
         erase_deals(beg->pair);

         //judges表
         erase_judges(beg->pair);
         erase_judgeorders(beg->pair);

         //arb
         erase_appeals(beg->pair);
         erase_arborders(beg->pair);

         beg++;
      }
   }
   //清理代币相关
   {
      auto state = overview_.get();
      for (auto &item : state.stocks)
      {
         std::string str_stock = item;
         str_stock = lower_str(str_stock);
         userbystock_index_t t(_self, name{str_stock}.value);
         auto it = t.begin();
         while (it != t.end())
         {

            erase_arbtasks(it->account);
            erase_judgetasks(it->account);
            erase_dhs(it->account);
            erase_balancelogs(it->account);
         }

         arbiter_index_t arbiters_(_self, name{str_stock}.value);
         auto it1 = arbiters_.begin();
         while (it1 != arbiters_.end())
         {

            erase_arbtasks(it1->account);
            erase_balancelogs(it1->account);
         }

         judger_index_t j_(_self, name{str_stock}.value);
         auto it2 = j_.begin();
         while (it2 != j_.end())
         {

            erase_arbtasks(it2->account);
            erase_balancelogs(it2->account);
         }
      }
   }

   //清理合约相关

   {
      //erase_balancelogs();
      erase_logs();
      //erase_markets();
      //overview_.remove();
   }
}
// 个人报酬:仲裁手续费/质押总金额×个人质押金额×个人胜利场次
ACTION otcexchange::avgarbfee(time_point_sec init_time)
{
   require_auth(_self);
   auto state = overview_.get();
   for (auto &item_str : state.stocks)
   {
      std::string str_stock = item_str;
      str_stock = lower_str(str_stock);
      arbpool_index_t t(_self, name{str_stock}.value);
      arbiter_index_t users_(_self, name{str_stock}.value);

      symbol stock = symbol(item_str, state.precisions[item_str]);
      asset zero = asset(0, stock);

      asset total_balance = zero;

      //在每天凌晨12:10运行,计算的上一天的数据
      auto yestoday_date = init_time.sec_since_epoch() - init_time.sec_since_epoch() % DAY_SECONDS - DAY_SECONDS;
      auto itr_pool = t.find(yestoday_date);
      if (itr_pool != t.end())
      {
         for (auto &item : itr_pool->personal_arb_fee)
         {
            auto itr_arber = users_.find(item.first.value);
            if (itr_arber != users_.end())
            {
               total_balance += itr_arber->balance;
            }
         }
      }

      if (total_balance.amount > 0 && itr_pool != t.end())
      {
         for (auto &item : itr_pool->personal_arb_fee)
         {
            auto itr_arber = users_.find(item.first.value);
            if (itr_arber != users_.end())
            {
               std::string detail = itr_arber->account.to_string();

               detail.append(" total_arb_fee:").append(itr_pool->total_arb_fee.to_string()).append(",");
               detail.append(" total_balance:").append(itr_pool->total_balance.to_string()).append(",");
               detail.append(" balance:").append(itr_arber->balance.to_string()).append(",");
               detail.append(" today_win_num:").append(std::to_string(itr_arber->today_win_num)).append(",");

               //send cc push action otcsystem award '[ "eoscngdszlgz", "10.0000 ADX"]' -p otcexchange
               asset lab = zero;
               t.modify(itr_pool, _self, [&](arbpool &a) {
                  lab = asset(itr_pool->total_arb_fee.amount / total_balance.amount * itr_arber->balance.amount * itr_arber->today_win_num, stock);
                  a.personal_arb_fee[item.first] = lab;
               });
               detail.append(" lab:").append(lab.to_string()).append("");

               send_action_award(_self, item.first, lab);

               inset_balancelog(item.first, stock.code(), BALANCE_MOD_AVGARBFEE, lab, zero, lab, itr_arber->balance + lab, detail);
            }
         }
      }
   }

   //这里再发一个新的延迟事物
   send_trans_avgarbfee();
}

ACTION otcexchange::defavgarbfee()
{
   require_auth(_self);
   send_trans_avgarbfee();
}

ACTION otcexchange::putadorder(const symbol_code &pair,
                               const std::string &side,
                               name user,
                               asset price,
                               asset amount,
                               asset amount_min,
                               asset amount_max,
                               const std::vector<uint64_t> &pay_accounts,
                               const std::vector<uint64_t> &pay_ways,
                               const std::vector<std::string> &pay_account_contexts,
                               const std::string &source)
{
   require_auth(user);
   check(can_today(user, CURRENT_SEC), "单日连续取消三次和发起仲裁，是买家，仲裁取消，是卖家，仲裁放币的，单日无法下单");

   auto itr_pair = get_open_market(pair);
   auto price_range = get_market_price_range(itr_pair, user);
   std::string ss{",价格浮动范围:["};
   ss.append(price_range.first.to_string()).append(",").append(price_range.second.to_string()).append("]");

   check(price > itr_pair->zero_money, ERR_MSG_PARAM_LT_ZERO(50210, price, ERR_MSG_PARAM_AD_PRICE_MUST_GT_ZERO));
   check(price >= price_range.first, std::string("广告交易价格小于合约配置最低价").append(itr_pair->price_min.to_string()).append(ss));  //提示用户卖出最低价不能小于XX
   check(price <= price_range.second, std::string("广告交易价格大于合约配置最高价").append(itr_pair->price_max.to_string()).append(ss)); //提示用户买入最高价不能大于XX

   check(pay_ways.size() > 0, "广告主必须提供一种法币支付方式");

   //判断交易数量是否在交易对配置区间
   check(amount > itr_pair->zero_stock, ERR_MSG_PARAM_LT_ZERO(50211, amount, ERR_MSG_PARAM_AD_AMOUNT_MUST_GT_ZERO));
   check(amount >= itr_pair->amount_min, "广告交易数量小于配置最小交易数量"); //提示用户最小交易数量不能小于XX
   check(amount <= itr_pair->amount_max, "广告交易数量大于配置最大交易数量"); //提示用户最大交易数量不能大于多少

   //判断吃单的交易限额是否合法，需满足 amount_min<=amount<=amount_max
   check(amount_min > itr_pair->zero_stock, ERR_MSG_PARAM_LE_ZERO(50212, amount_min, ERR_MSG_PARAM_AD_AMOUNT_MIN_MUST_GT_ZERO));
   check(amount_max > itr_pair->zero_stock, ERR_MSG_PARAM_LE_ZERO(50213, amount_max, ERR_MSG_PARAM_AD_AMOUNT_MAX_MUST_GT_ZERO));

   check(amount_min <= amount_max, ERR_MSG_PARAM_LE(50215, amount_min, amount_max, ERR_MSG_PARAM_AD_AMOUNT_MAX_MUST_GE_MIN));
   check(amount_max <= amount, ERR_MSG_PARAM_LE(50216, amount_max, amount, ERR_MSG_PARAM_AD_AMOUNT_MUST_GE_AMOUNT_MAX));

   check((side == MARKET_ORDER_SIDE_ASK_STR || side == MARKET_ORDER_SIDE_BID_STR), ERR_MSG_SIDE);

   auto side_int = side_to_uint(side);

   std::string balance_type = (side_int == MARKET_ORDER_SIDE_ASK) ? BALANCE_MOD_PUTMK_ASK : BALANCE_MOD_PUTMK_BID;

   adorder_index_t adtable_ = get_adtable(pair, side);

   auto ad_id = adtable_.available_primary_key();
   auto fee_rate_pair = get_user_fee_rate(user, pair, MARKET_ROLE_MAKER);

   //是卖，判断余额够不够
   if (side_int == MARKET_ORDER_SIDE_ASK)
   {
      //提前冻结手续费
      asset ask_fee = cal_fee(amount, fee_rate_pair.first); //get_fee(user, pair, amount, MARKET_ORDER_SIDE_ASK, MARKET_ROLE_MAKER);
      asset total = amount + ask_fee;
      freeze_stock(user, name{TOKEN_TEMP_ACCOUNT}, total, USER_TYPE_OTC, std::string("广告卖币,需要冻结,把钱转到合约帐号:").append(total.to_string()));

      std::string detail(user.to_string());
      detail.append(" put ask ad_id:").append(std::to_string(ad_id)).append(",");
      detail.append("amount:").append(amount.to_string()).append(",");
      detail.append("price:").append(price.to_string()).append(",");
      detail.append("maker_ask_fee_rate:").append(std::to_string(fee_rate_pair.first)).append(",");
      detail.append("ask_fee:").append(ask_fee.to_string()).append(",");
      detail.append("total freeze:").append(total.to_string()).append("");

      inset_balancelog(user, itr_pair->stock.code(), balance_type, amount, ask_fee, total, itr_pair->zero_stock, detail);
      print("是卖，可用余额足"); //这里要发送一笔转账,也就是先要冻结
   }
   else
   {
      inset_balancelog(user, itr_pair->stock.code(), balance_type, itr_pair->zero_stock, itr_pair->zero_stock, itr_pair->zero_stock, itr_pair->zero_stock, " 发布买广告，不预先冻结和收取手续费");
   }

   adtable_.emplace(_self, [&](adorder &order) {
      order.id = ad_id;
      order.user = user;                           //订单所属用户，注意是name类型
      order.pair = pair;                           //
      order.ctime = current_time_point();          //订单创建时间，精确到微秒
      order.utime = order.ctime;                   //订单更新时间，精确到微秒
      order.status = AD_STATUS_ONTHESHELF;         //订单状态，上架中
      order.status_str = AD_STATUS_ONTHESHELF_STR; //订单状态，上架中
      order.nickname = get_user_nickname(user);
      order.side = side_int;                           //买卖类型，1卖 2买
      order.type = MARKET_ORDER_TYPE_LIMIT;            //订单类型，1限价 2市价
      order.role = MARKET_ROLE_MAKER;                  //订单类型，1挂单 2吃单
      order.price = price;                             //订单交易价格
      order.amount = amount;                           //订单交易数量
      order.amount_min = amount_min;                   //订单最小成交数量,默认是2
      order.amount_max = amount_max;                   //订单最小成交数量,默认是2
      order.maker_ask_fee_rate = fee_rate_pair.first;  //吃单的手续费率
      order.maker_bid_fee_rate = fee_rate_pair.second; //挂单的手续费率
      order.left = amount;                             //剩余多少数量未成交
      order.freeze = itr_pair->zero_stock;             //冻结的stock或者money
      order.deal_fee = itr_pair->zero_stock;           //累计的交易手续费
      order.deal_stock = itr_pair->zero_stock;         //累计的交易sotck数量
      order.deal_money = itr_pair->zero_money;         //累计的交易money
      order.pay_accounts = pay_accounts;               //支付方式
      order.pay_ways = pay_ways;
      order.pay_account_contexts = pay_account_contexts;
      order.source = source; //备注信息，订单来源
   });

   update_user_ad_unfill_num(user, 1);
   inup_userbystock(itr_pair->stock.code(), user);
}

ACTION otcexchange::offad(const symbol_code &pair,
                          const std::string &side,
                          uint64_t ad_id,
                          const std::string &reason)
{
   //如果是卖币的广告单，不能取消，因为买币方可能已经支付了法币

   auto adtable_ = get_adtable(pair, side);
   auto it = adtable_.require_find(ad_id, ORDER_NOT_EXIST_STR);
   auto user = it->user;
   require_auth(user); //鉴权
   check(it->status != AD_STATUS_MAN_OFFTHESHELF, "ad order 已经被手动下架");

   //哪种条件才能取下架合约
   //检查当前的广告下面的吃单最新状态，看是否都完成了或者都取消了
   //转换大小写
   deal_index_t dealtable_(_self, name{lower_str(pair.to_string())}.value);

   for (auto &id : it->vec_deal)
   {
      auto d = dealtable_.require_find(id, "吃单找不到");
      //ad下面挂的吃单都是这几种，才能撤销广告
      check((d->status >= DEAL_STATUS_UNPAID_MAN_CANCEL), std::string("有正在交易的taker deal,不能下架,dealstatus:").append(d->status_str));
   }

   std::string balance_type = (it->side == MARKET_ORDER_SIDE_ASK) ? BALANCE_MOD_OFFMK_ASK : BALANCE_MOD_OFFMK_BID;

   //是撤销广告单，是卖币方才有解冻
   if (it->side == MARKET_ORDER_SIDE_ASK)
   {
      auto itr_pair = get_market(pair);
      asset ask_fee = cal_fee(it->left, it->maker_ask_fee_rate); //get_fee(user, pair, it->left, MARKET_ORDER_SIDE_ASK, MARKET_ROLE_MAKER);
      asset total = ask_fee + it->left;

      std::string memo = std::string("卖币广告手动下架,需要解冻本金和手续费,把钱转给用户:");
      memo.append(total.to_string());
      unfreeze_stock(name{TOKEN_TEMP_ACCOUNT}, user, total, USER_TYPE_OTC, memo);
      print("卖币广告手动下架,需要解冻,把钱转给用户ok");
      std::string detail(user.to_string());
      detail.append("man off ask ad_id:").append(std::to_string(ad_id)).append(",");
      detail.append("left:").append(it->left.to_string()).append(",");
      detail.append("price:").append(it->price.to_string()).append(",");
      detail.append("maker_ask_fee_rate:").append(std::to_string(it->maker_ask_fee_rate)).append(",");
      detail.append("ask_fee:").append(ask_fee.to_string()).append(",");
      detail.append("total unfreeze:").append(total.to_string()).append("");
      inset_balancelog(user, itr_pair->stock.code(), balance_type, it->left, ask_fee, total, itr_pair->zero_stock, detail);
   }
   else
   {
      auto itr_pair = get_market(pair);
      inset_balancelog(user, itr_pair->stock.code(), balance_type, itr_pair->zero_stock, itr_pair->zero_stock, itr_pair->zero_stock, itr_pair->zero_stock, "下架买广告不需要解冻资产");
   }

   adtable_.modify(it, _self, [&](adorder &ad) {
      ad.status = AD_STATUS_MAN_OFFTHESHELF;         //手动下架
      ad.status_str = AD_STATUS_MAN_OFFTHESHELF_STR; //手动下架
      ad.utime = current_time_point();
      ad.source.append("手动下架").append(reason);
   });

   //todo 用户每手动下架一个广告，记录一次
   update_user_ad_unfill_num(user, -1);
}

ACTION otcexchange::puttkorder(const symbol_code &pair,
                               const std::string &side,
                               name user,
                               asset price,
                               asset amount,
                               uint64_t ad_id,
                               const std::vector<uint64_t> &pay_accounts,
                               const std::vector<uint64_t> &pay_ways,
                               const std::vector<std::string> &pay_account_contexts,
                               const std::string &source)

{

   require_auth(user);
   check(can_today(user, CURRENT_SEC), "单日连续取消三次和发起仲裁，是买家，仲裁取消，是卖家，仲裁放币的，单日无法下单");
   check(!check_user_deal_not_fill(user), user.to_string().append(" 有未完成的订单，不能在下单"));

   auto itr_pair = get_open_market(pair);

   check(price > itr_pair->zero_money, ERR_CHECK_PRICE_GREAT_ZERO);   //要求吃单价>0
   check(amount > itr_pair->zero_stock, ERR_CHECK_AMOUNT_GREAT_ZERO); //吃单数量>0

   //交易对的属性比对

   //校验side是否合法
   check((side == MARKET_ORDER_SIDE_ASK_STR || side == MARKET_ORDER_SIDE_BID_STR), ERR_MSG_SIDE);

   //1.找到对手方订单的买卖的方向
   std::string mk_side = (side == MARKET_ORDER_SIDE_ASK_STR) ? MARKET_ORDER_SIDE_BID_STR : MARKET_ORDER_SIDE_ASK_STR;
   adorder_index_t adtable_ = get_adtable(pair, mk_side);

   //要求挂单必须存在
   auto mk_it = adtable_.require_find(ad_id, MAKER_ORDER_NOR_EXIST_STR);
   check(mk_it->status == AD_STATUS_ONTHESHELF, "广告必须是上架中"); //todo 提示翻译

   //获取挂单的用户
   const auto &mk_user = mk_it->user;
   //必须不是自成交
   check(mk_user != user, ERR_FORBID_SELF_EXCHANGE); //不能是自成交

   //下单数量必须小于等于广告未交易数量
   check(amount <= mk_it->left, ERR_NOT_ENOUGH_TOKEN_TO_SELL); //提示广告剩余数量不足,请重新下单
   //判断数量是否在广告单的限额内提示用户下单数量应在[]之间
   check(amount >= mk_it->amount_min, "吃单数量小于广告单最小交易数量");
   check(amount <= mk_it->amount_max, "吃单交易数量大于广告单最大交易数量");

   auto tk_side = side_to_uint(side);

   //上面判断都通过，生成一个成交记录，也就是吃单详情,属于合约，scope是交易对

   std::string scope = pair.to_string();
   scope = lower_str(scope);
   deal_index_t dealtable_(_self, name{scope}.value);
   uint64_t deal_id = dealtable_.available_primary_key();

   std::string detail(user.to_string());
   detail.append("put take->side:").append(side).append(",deal_id:").append(std::to_string(deal_id)).append(",ad_id:").append(std::to_string(ad_id)).append(",");
   detail.append("amount:").append(amount.to_string()).append(",");
   detail.append("price:").append(price.to_string()).append(",");

   if (tk_side == MARKET_ORDER_SIDE_BID)
   {
      check(price >= mk_it->price, "我出的买价必须>=卖价,才能撮合");
      print("我是买币，我出的买价>=卖家的卖价");
   }
   else
   {

      //我是卖币方,看我是不是还有币可卖,如果有,则直接冻结,这里发送一个内联调用action,如果执行成功，就继续，失败，就不往下走
      check(price <= mk_it->price, "我的卖价必须<=买价,才能撮合");
      print("我是卖币，我出的卖价<=买家的买价");

      //冻结我的币,提示用户您的余额不足
      freeze_stock(user, name{TOKEN_TEMP_ACCOUNT}, amount, USER_TYPE_OTC, std::string("taker 卖币,冻结").append(amount.to_string()));

      print("我要卖币给广告方，可用余额够", user.to_string()); //这里要发送一笔转账,也就是先要冻结

      detail.append("total freeze:").append(amount.to_string()).append("");
      inset_balancelog(user, itr_pair->stock.code(), BALANCE_MOD_TK_ASK, amount, itr_pair->zero_stock, amount, itr_pair->zero_stock, detail);
   }

   auto now = current_time_point();
   auto pr = std::pow(10, amount.symbol.precision());
   auto quota = (price * amount.amount) / pr; //算一下交易额是多少,也就是买币方要支付的法币
   auto mk_side_uint = side_to_uint(mk_side);

   auto tk_dh_id = new_dh_history(user, deal_id, ad_id, pair, tk_side, MARKET_ROLE_TAKER, price, amount, quota, now, now);
   auto mk_dh_id = new_dh_history(mk_user, deal_id, ad_id, pair, mk_side_uint, MARKET_ROLE_MAKER, price, amount, quota, now, now);

   //auto tk_pair = get_user_fee_rate(user, pair, MARKET_ROLE_TAKER);
   //auto mk_pair = get_user_fee_rate(mk_user, pair, MARKET_ROLE_MAKER);

   auto pay_timeout_sender_id = get_avail_senderid(detail, "puttkorder,支付超时取消事务");

   dealtable_.emplace(_self, [&](deal &deal) {
      deal.id = deal_id;
      deal.taker_id = tk_dh_id;
      deal.maker_id = mk_dh_id;
      deal.user = user;                    //订单所属用户，注意是name类型
      deal.side = tk_side;                 //买卖类型，1卖 2买
      deal.type = MARKET_ORDER_TYPE_LIMIT; //订单类型，1限价 2市价
      deal.role = MARKET_ROLE_TAKER;       //订单类型，1挂单 2吃单
      deal.ctime = now;                    //订单创建时间，精确到微秒
      deal.utime = deal.ctime;             //订单更新时间，精确到微秒

      deal.maker_user = mk_user;
      deal.maker_order_id = ad_id;
      deal.tk_nickname = get_user_nickname(user);
      deal.mk_nickname = mk_it->nickname;

      deal.price = price;   //订单交易价格
      deal.amount = amount; //订单交易数量

      deal.quota = quota;

      if (tk_side == MARKET_ORDER_SIDE_ASK)
      {
         deal.ask_pay_accounts = pay_accounts;
         deal.ask_pay_ways = pay_ways;
         deal.ask_pay_account_contexts = pay_account_contexts;

         deal.bid_pay_accounts = mk_it->pay_accounts;
         deal.bid_pay_ways = mk_it->pay_ways;
         deal.bid_pay_account_contexts = mk_it->pay_account_contexts;

         //deal.ask_fee = cal_fee(amount, tk_pair.first); //2个不同精度的asset能不能互相需要验证一下
         //deal.bid_fee = cal_fee(amount, mk_pair.second);
      }
      else
      {
         deal.bid_pay_accounts = pay_accounts;
         deal.bid_pay_ways = pay_ways;
         deal.bid_pay_account_contexts = pay_account_contexts;

         deal.ask_pay_accounts = mk_it->pay_accounts;
         deal.ask_pay_ways = mk_it->pay_ways;
         deal.ask_pay_account_contexts = mk_it->pay_account_contexts;

         //deal.ask_fee = cal_fee(amount, mk_pair.first);
         //deal.bid_fee = cal_fee(amount, tk_pair.second);
      }

      deal.ask_fee = cal_fee(amount, mk_it->maker_ask_fee_rate); //2个不同精度的asset能不能互相需要验证一下
      deal.bid_fee = cal_fee(amount, mk_it->maker_bid_fee_rate);

      deal.fiat_pay_method = FIAT_PAY_UNKOWN;
      deal.fiat_account = "";

      deal.status = DEAL_STATUS_UNPAID;         //待支付状态
      deal.status_str = DEAL_STATUS_UNPAID_STR; //待支付状态
      deal.pay_timeout = itr_pair->pay_timeout;
      deal.def_cancel_time = itr_pair->def_cancel_time;
      deal.def_playcoin_time = itr_pair->def_playcoin_time;
      deal.pay_timeout_sender_id = pay_timeout_sender_id;
      deal.self_playcoin_sender_id = 0;
      deal.arb_sender_id = 0;
      deal.pair = pair;

      std::string tip(user.to_string());
      tip.append(" ").append(side).append(" 吃单:").append(DEAL_STATUS_UNPAID_STR);
      deal.source = source;
      deal.source.append(tip);

      deal.appeal_side = MARKET_ORDER_SIDE_INVAID;
      deal.arb_failer = name{"unkown"};
      deal.judge_failer = name{"unkown"};
   });

   std::string fmt{user.to_string()};
   fmt.append("take->side:").append(side);
   fmt.append(",price:").append(price.to_string());
   fmt.append(",amount:").append(amount.to_string());

   uint8_t new_ad_status = 0;

   //修改广告挂单的状态
   adtable_.modify(mk_it, _self, [&itr_pair, &deal_id, &amount, &tk_side, &fmt, &new_ad_status](adorder &o) {
      o.vec_deal.emplace_back(deal_id);
      o.source.append("|").append(fmt);
      o.utime = current_time_point();
      o.left = o.left - amount; //下单成功,减少广告单可交易数量
      //mk是卖币方
      o.freeze = o.freeze + amount; //o.freeze是在处于交易中的币正在等待对方法币支付
      //otcexchange::update_ad_status(o, itr_pair);
      o.update_ad_status(itr_pair);
      new_ad_status = o.status;
   }

   );
   if (new_ad_status == AD_STATUS_FINISHED)
   {
      update_user_ad_unfill_num(mk_user, -1);
   }

   //待支付状态,需要发送一个延时事物,法币支付超时自动取消,发送一个延迟事物;取消交易 deal
   //默认是taker发起的延迟事物

   transaction t{};

   //name who = (tk_side == MARKET_ORDER_SIDE_ASK) ? mk_user : user; //找到谁是买方

   t.actions.emplace_back(
       permission_level(user, name{"active"}),
       _self,
       name{"defcldeal"},
       std::make_tuple(pair, user, deal_id, DEAL_STATUS_UNPAID_TIMEOUT_CANCEL, "买方支付超时取消deal"));

   t.delay_sec = itr_pair->pay_timeout;

   t.send(pay_timeout_sender_id, _self);
   printf("Sent with a delay of %d ", t.delay_sec);
   user_deal_not_fill(user, detail); //尚有未处理完的deal
   inup_userbystock(itr_pair->stock.code(), user);
}

ACTION otcexchange::mancldeal(const symbol_code &pair, name bider, uint64_t deal_id, const std::string &reason)
{
   require_auth(bider);
   auto str_pair = pair.to_string();
   str_pair = lower_str(str_pair);
   deal_index_t dealtable_(_self, name{str_pair}.value);
   auto it = dealtable_.require_find(deal_id, "deal id 不存在");

   //检查一下status是否正确
   bool not_appeal = it->status == DEAL_STATUS_UNPAID || it->status == DEAL_STATUS_PAID_WAIT_PLAYCOIN;
   bool appeal = it->status == DEAL_STATUS_PAID_APPEAL_ASK || it->status == DEAL_STATUS_PAID_APPEAL_BID || it->status == DEAL_STATUS_PAID_APPEAL_ALL;

   check(not_appeal || appeal, "只有在未支付状态或者已付款待放币（误点）状态或者仲裁还没开始状态下才能手动取消交易");
   if (appeal)
   {
      arb_can_cmd(pair, it, "仲裁没开始前手动取消deal");
   }

   bool who_taker_bid = (bider == it->user) && (it->side == MARKET_ORDER_SIDE_BID);
   bool who_maker_bid = (bider == it->maker_user) && (it->side == MARKET_ORDER_SIDE_ASK);
   check(who_taker_bid || who_maker_bid, "只有在买方未支付状态下才能手动取消交易");

   std::string new_reason("交易对:");
   new_reason.append(pair.to_string()).append("被买家").append(bider.to_string()).append("手动取消deal_id:").append(std::to_string(deal_id)).append(",原因是").append(reason);
   rollback_deal(dealtable_, it, DEAL_STATUS_UNPAID_MAN_CANCEL, new_reason);
   update_userdaystat(bider, CURRENT_SEC, EXCHANGE, 1, new_reason);
}

//法币支付发点击了已支付
ACTION otcexchange::paydeal(const symbol_code &pair, name bider, uint64_t deal_id, uint64_t fiat_pay_method, uint64_t pmt_id) //支付
{
   require_auth(bider);

   auto str_pair = pair.to_string();
   str_pair = lower_str(str_pair);
   deal_index_t dealtable_(_self, name{str_pair}.value);
   auto it = dealtable_.require_find(deal_id, "吃单找不到");

   //检查一下status是否正确
   check(it->status == DEAL_STATUS_UNPAID, "只有在未支付状态下才能点击法币支付");

   //校验who是买家
   bool who_taker_bid = (bider == it->user) && (it->side == MARKET_ORDER_SIDE_BID);
   bool who_maker_bid = (bider == it->maker_user) && (it->side == MARKET_ORDER_SIDE_ASK);
   check(who_taker_bid || who_maker_bid, "只有在买方未支付状态下才能点击法币支付");

   update_dh_status(it->user, it->taker_id, DEAL_STATUS_PAID_WAIT_PLAYCOIN);
   update_dh_status(it->maker_user, it->maker_id, DEAL_STATUS_PAID_WAIT_PLAYCOIN);

   name asker = it->side == MARKET_ORDER_SIDE_BID ? it->maker_user : it->user;
   std::string asker_pay_account_context = std::to_string(pmt_id);
   asker_pay_account_context.append("|");
   asker_pay_account_context.append(get_user_pay_accout_content(asker, pmt_id));

   dealtable_.modify(it, _self, [&](deal &d) {
      d.status = DEAL_STATUS_PAID_WAIT_PLAYCOIN;
      d.status_str = DEAL_STATUS_PAID_WAIT_PLAYCOIN_STR;
      d.utime = current_time_point();
      d.fiat_pay_method = fiat_pay_method;
      d.fiat_account = asker_pay_account_context;
      d.source.append("|买方").append(bider.to_string()).append("已支付卖方").append(asker_pay_account_context).append(",").append(DEAL_STATUS_PAID_WAIT_PLAYCOIN_STR);
   });

   //取消异步事物
   auto res = cancel_deftransaction(it->pay_timeout_sender_id, "买方已经付款，取消支付超时自动取消任务");
   update_userdaystat(bider, CURRENT_SEC, EXCHANGE, 0, "买方已经付款,终结连续取消次数"); //todo
}

ACTION otcexchange::selfplaycoin(const symbol_code &pair, name asker, uint64_t deal_id, const std::string &reason, bool right_now) //放币操作
{

   require_auth(asker);
   check(right_now, "不接受卖方延迟放币");

   auto str_pair = pair.to_string();
   str_pair = lower_str(str_pair);
   deal_index_t dealtable_(_self, name{str_pair}.value);
   auto it = dealtable_.require_find(deal_id, "吃单找不到");

   bool appeal = it->status == DEAL_STATUS_PAID_APPEAL_ASK || it->status == DEAL_STATUS_PAID_APPEAL_BID || it->status == DEAL_STATUS_PAID_APPEAL_ALL;
   check(it->status == DEAL_STATUS_PAID_WAIT_PLAYCOIN || appeal, "只有买方法币支付了，才能放币");
   if (appeal)
   {
      arb_can_cmd(pair, it, "仲裁没开始前卖方运行放币");
   }

   //校验who是不是卖币者
   bool who_taker_bid = (asker == it->user) && (it->side == MARKET_ORDER_SIDE_ASK);
   bool who_maker_bid = (asker == it->maker_user) && (it->side == MARKET_ORDER_SIDE_BID);
   check(who_taker_bid || who_maker_bid, "我是卖币者，只能在买方已经支付法币的前提下放币");

   update_dh_status(it->user, it->taker_id, DEAL_STATUS_PAID_PLAYCOIN_ING);
   update_dh_status(it->maker_user, it->maker_id, DEAL_STATUS_PAID_PLAYCOIN_ING);

   dealtable_.modify(it, _self, [&](deal &d) {
      d.utime = current_time_point();
      d.status = DEAL_STATUS_PAID_PLAYCOIN_ING;
      d.status_str = DEAL_STATUS_PAID_PLAYCOIN_ING_STR;
      d.source.append("|卖方").append(asker.to_string()).append("主动放币:").append(DEAL_STATUS_PAID_PLAYCOIN_ING_STR);
   });

   //是否立即放币

   if (right_now)
   {
      std::string new_reason = reason;
      new_reason.append("卖方").append(asker.to_string()).append("主动立即放币:");

      commit_deal(asker, dealtable_, it, DEAL_STATUS_SUCCESS_FINISHED, new_reason);

      //update_userdaystat(asker, CURRENT_SEC, EXCHANGE, 0, new_reason); //统计数据
   }
   else
   { //延迟事物放币
      auto now = current_time_point().sec_since_epoch();
      dealtable_.modify(it, _self, [&](deal &d) {
         d.self_playcoin_sender_id = now;
      });

      std::string new_reason = reason;
      new_reason.append("卖方").append(asker.to_string()).append("主动延迟放币:");

      //放币XX后生效

      transaction t{};

      t.actions.emplace_back(
          permission_level(asker, name{"active"}),
          _self,
          name{"defcmdeal"},
          std::make_tuple(asker, pair, asker, deal_id, DEAL_STATUS_SUCCESS_FINISHED, new_reason));

      auto itr_pair = get_market(pair);

      t.delay_sec = itr_pair->self_playcoin_time;

      t.send(now, _self);
   }
}

// 定时任务取消deal，who 默认是taker zz  push action otcexchange defcldeal '["ADXCNY","dabaicai",7,62,"taker是卖币吃单，测试"]' -p dabaicai@active
ACTION otcexchange::defcldeal(const symbol_code &pair, name cmder, uint64_t deal_id, uint8_t status, const std::string &reason)
{
   require_auth(cmder);
   auto str_pair = pair.to_string();
   str_pair = lower_str(str_pair);
   deal_index_t dealtable_(_self, name{str_pair}.value);

   auto it = dealtable_.require_find(deal_id, "deal id 不存在");
   std::string msg("异步事件:defcldeal,");
   name bider = (it->side == MARKET_ORDER_SIDE_ASK) ? it->maker_user : it->user;
   msg.append("pair:").append(pair.to_string()).append(",");
   msg.append("cmder:").append(cmder.to_string()).append(",");
   msg.append("bider:").append(bider.to_string());
   msg.append("deal_id:").append(std::to_string(deal_id)).append(",");
   msg.append("status:").append(std::to_string(status)).append(",");
   msg.append("reason:").append(reason).append(",");

   //检查一下status是否正确
   bool b1 = it->status == DEAL_STATUS_UNPAID && status == DEAL_STATUS_UNPAID_TIMEOUT_CANCEL;
   bool b2 = it->status == DEAL_STATUS_PAID_ARBIARATE_CANCEL && status == DEAL_STATUS_ARB_CANCEL_FINISHED;
   if (b1)
   {
      msg.append("买方超时未付款取消交易延迟事务,");
      update_userdaystat(bider, CURRENT_SEC, EXCHANGE, 1, msg);
   }
   if (b2)
   {
      msg.append("仲裁取消放币延迟事务,");
   }
   check(b1 || b2, "只有在未支付状态或者仲裁取消中状态下才能取消交易");

   rollback_deal(dealtable_, it, status, reason);

   log(msg);
}

//定时任务提交deal，who是放币者
ACTION otcexchange::defcmdeal(name cmder, const symbol_code &pair, name asker, uint64_t deal_id, uint8_t status, const std::string &reason)
{
   require_auth(cmder);

   auto str_pair = pair.to_string();
   str_pair = lower_str(str_pair);
   deal_index_t dealtable_(_self, name{str_pair}.value);
   auto it = dealtable_.require_find(deal_id, "deal id 不存在");

   bool b1 = it->status == DEAL_STATUS_PAID_PLAYCOIN_ING && status == DEAL_STATUS_SUCCESS_FINISHED;
   check(b1, "deal卖方放币中->放币完成");
   commit_deal(asker, dealtable_, it, status, reason);
}

ACTION otcexchange::cmddefcmdeal(name cmder, const symbol_code &pair, name asker, uint64_t deal_id, uint8_t status, const std::string &reason)
{

   require_auth(cmder); //下达命令的人
   std::string msg("异步事件:defcldeal,");
   msg.append("pair:").append(pair.to_string()).append(",");
   msg.append("cmder:").append(cmder.to_string()).append(",");
   msg.append("asker:").append(asker.to_string()).append(",");
   msg.append("deal_id:").append(std::to_string(deal_id)).append(",");
   msg.append("status:").append(std::to_string(status)).append(",");
   msg.append("reason:").append(reason).append(",");

   auto str_pair = pair.to_string();
   str_pair = lower_str(str_pair);
   deal_index_t dealtable_(_self, name{str_pair}.value);
   auto it = dealtable_.require_find(deal_id, "deal id 不存在");

   //检查一下status是否正确
   bool b1 = it->status == DEAL_STATUS_PAID_ARBIARATE_PALYCOIN && status == DEAL_STATUS_ARB_PLAYCOIN_FINISHED;
   check(b1, "deal 仲裁放币中->仲裁放币完成");
   //仲裁放币才有手续费拿
   commit_deal(asker, dealtable_, it, status, reason);

   log(msg, LOG_TYPE_DEF_TRANS);
}

// 等待放币/仲裁中 - > 伸诉/仲裁中 买卖双方第一次上传申诉材料

ACTION otcexchange::putappeal(name who,
                              const std::string &side,
                              const symbol_code &pair,
                              uint64_t deal_id,
                              const std::vector<std::string> &vec_contacts,
                              const std::string &reason,
                              const std::string &desc,
                              const std::vector<std::string> &vec_img,
                              const std::vector<std::string> &vec_video,
                              const std::string &source,
                              const std::string &email)
{
   require_auth(who);

   check((side == MARKET_ORDER_SIDE_ASK_STR || side == MARKET_ORDER_SIDE_BID_STR), ERR_MSG_SIDE);

   auto str_pair = pair.to_string();
   str_pair = lower_str(str_pair);
   deal_index_t dealtable_(_self, name{str_pair}.value);

   auto side_uint = side_to_uint(side);
   auto itr_deal = dealtable_.require_find(deal_id, "要申诉的deal不存在");
   //自己主动放币
   check(itr_deal->status == DEAL_STATUS_PAID_WAIT_PLAYCOIN ||
             itr_deal->status == DEAL_STATUS_PAID_ARBIARATE_ING,
         "订单状态已为申诉中，不能提交申诉");

   check(who == itr_deal->user || who == itr_deal->maker_user, "申诉者不是taker也不是maker");

   if (who == itr_deal->user) //taker上传材料
   {
      check(itr_deal->side == side_uint, "who 作为taker，side跟deal里的方向不一样");
   }
   else if (who == itr_deal->maker_user)
   {
      auto maker_side = (itr_deal->side == MARKET_ORDER_SIDE_ASK) ? MARKET_ORDER_SIDE_BID : MARKET_ORDER_SIDE_ASK;
      check(maker_side == side_uint, "who 作为maker，side跟deal里的方向不一样");
   }
   else
   {
      check(false, "申诉者必须是deal的参与者");
   }

   //先看是否存在这个申诉
   xappeal_index_t appeals(_self, name{str_pair}.value);
   auto itr_appeal = appeals.find(deal_id);
   auto now = time_point_sec(current_time_point().sec_since_epoch());

   if (itr_appeal == appeals.end())
   {

      appeals.emplace(_self, [&](xappeal &a) {
         a.id = deal_id;
         a.deal_id = deal_id;
         a.arborder_id = deal_id;
         a.pair = pair;
         a.initiator_side = side_uint;
         a.ctime = now;
         a.utime = now;
         a.source = source;

         if (side_uint == MARKET_ORDER_SIDE_ASK)
         {
            a.ask_user = who;
            a.ask_content = "";
            a.ask_contacts = vec_contacts;
            a.ask_desc = desc;
            a.ask_reason = reason;
            a.ask_imgs = vec_img;
            a.ask_videos = vec_video;
            a.ask_s1 = email;
         }
         else
         {
            a.bid_user = who;
            a.bid_content = "";
            a.bid_contacts = vec_contacts;
            a.bid_desc = desc;
            a.bid_reason = reason;
            a.bid_imgs = vec_img;
            a.bid_videos = vec_video;
            a.bid_s1 = email;
         }
      });
      auto new_status = (side_uint == MARKET_ORDER_SIDE_ASK) ? DEAL_STATUS_PAID_APPEAL_ASK : DEAL_STATUS_PAID_APPEAL_BID;
      auto new_status_str = (side_uint == MARKET_ORDER_SIDE_ASK) ? DEAL_STATUS_PAID_APPEAL_ASK_STR : DEAL_STATUS_PAID_APPEAL_BID_STR;

      dealtable_.modify(itr_deal, _self, [&side_uint, &new_status, &new_status_str, &who](deal &d) {
         d.status = new_status;
         d.status_str = new_status_str;
         d.source.append("|").append(who.to_string()).append("发起申诉:").append(new_status_str);
         d.appeal_side = side_uint;
         d.utime = current_time_point();
      });

      update_dh_status(itr_deal->user, itr_deal->taker_id, new_status);
      update_dh_status(itr_deal->maker_user, itr_deal->maker_id, new_status);

      //新增一个申诉，这个时候要生成一个仲裁订单，然后这个订单下面又有很多仲裁log，来记录仲裁员的仲裁记录

      std::set<name> arbiters; //分配仲裁员

      auto itr_pair = get_market(pair);
      get_avail_arbiter(itr_pair->stock.code(), arbiters, GROUP_ARBPEOPLE_NUM, now, itr_deal);

      check(arbiters.size() >= GROUP_ARBPEOPLE_MIN_NUM, "系统仲裁员个数<=7,无法分配仲裁任务");

      arborder_index_t arbs(_self, name{str_pair}.value); //订单表

      arbs.emplace(_self, [&](arborder &a) {
         a.id = deal_id;
         a.ctime = now;
         a.utime = now;
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
         a.status = APPEAL_STATUS_CREATED;
         a.status_str = APPEAL_STATUS_CREATED_STR;
         a.judge_result = JUDGE_UNKOWN; //复审的结果
         a.source = source;
      });

      std::map<name, uint64_t> map_person_task;
      std::map<name, uint8_t> map_person_pick;

      for (auto arbiter : arbiters)
      {
         arbtask_index_t arbtasks(_self, arbiter.value); //表名，按用户分表，不是按pair分表
         arbtasks.emplace(_self, [&](arbtask &log) {
            log.id = arbtasks.available_primary_key();
            log.side = side_uint;
            log.appeal_time = now;
            log.ctime = now;
            log.utime = log.ctime;
            log.pair = pair;
            log.stock = itr_pair->stock.code();
            log.money = itr_pair->money.code();
            log.amount = itr_deal->amount;
            log.price = itr_deal->price;
            log.quota = itr_deal->quota;
            log.arborder_id = deal_id;
            log.deal_id = deal_id;
            log.appeal_id = deal_id;
            log.self_arbit_result = ARBIT_UNKOWN;
            log.group_arbit_result = ARBIT_UNKOWN;
            log.group_no_num = 0;
            log.group_yes_num = 0;
            log.labor_fee = itr_pair->zero_stock;
            log.status = ARBTASK_STATUS_CREATED;
            log.status_str = ARBTASK_STATUS_CREATED_STR;
            log.source = source;
            map_person_task.emplace(arbiter, log.id);
            map_person_pick.emplace(arbiter, ARBIT_UNKOWN);
         });

         //每个用户仲裁总数+1,然后每个仲裁员处于工作状态
         update_arbiter_total_num(itr_pair->stock.code(), arbiter);
      }

      //写回
      auto itr_arborder = arbs.require_find(deal_id, "找不到仲裁订单表");
      arbs.modify(itr_arborder, _self, [&map_person_task, &map_person_pick](arborder &a) {
         a.map_person_task = map_person_task;
         a.map_person_pick = map_person_pick;
      });
   }
   else //对手房已经上传过申诉材料
   {
      check((itr_appeal->initiator_side == MARKET_ORDER_SIDE_ASK && side_uint == MARKET_ORDER_SIDE_BID) ||
                (itr_appeal->initiator_side == MARKET_ORDER_SIDE_BID && side_uint == MARKET_ORDER_SIDE_ASK),
            "已经发起过申诉,不允许修改");
      //说明之前已经有过申诉，这个应该是对手方的申诉
      appeals.modify(itr_appeal, _self, [&](xappeal &a) {
         a.utime = time_point_sec(current_time_point().sec_since_epoch());
         if (itr_appeal->initiator_side == MARKET_ORDER_SIDE_ASK)
         {
            a.bid_user = who;
            a.bid_content = "";
            a.bid_contacts = vec_contacts;
            a.bid_desc = desc;
            a.bid_reason = reason;
            a.bid_imgs = vec_img;
            a.bid_videos = vec_video;
         }
         else
         {
            a.ask_user = who;
            a.ask_content = "";
            a.ask_contacts = vec_contacts;
            a.ask_desc = desc;
            a.ask_reason = reason;
            a.ask_imgs = vec_img;
            a.ask_videos = vec_video;
         }
         a.source.append("|对手方上传申诉材料").append(source);
      });

      if (itr_deal->status != DEAL_STATUS_PAID_ARBIARATE_ING) //仲裁还没有开始，买卖双方都可以上传资料
      {
         dealtable_.modify(itr_deal, _self, [&side_uint, &who](deal &d) {
            d.status = DEAL_STATUS_PAID_APPEAL_ALL;
            d.status_str = DEAL_STATUS_PAID_APPEAL_ALL_STR;
            d.source.append("|").append(who.to_string()).append("发起申诉:").append(DEAL_STATUS_PAID_APPEAL_ALL_STR);
            d.utime = current_time_point();
         });

         update_dh_status(itr_deal->user, itr_deal->taker_id, DEAL_STATUS_PAID_APPEAL_ALL);
         update_dh_status(itr_deal->maker_user, itr_deal->maker_id, DEAL_STATUS_PAID_APPEAL_ALL);
      }
   }
}

ACTION otcexchange::arbdeal(name arbiter, const symbol_code &pair, uint64_t deal_id, uint8_t choice, const std::string &reason)
{
   require_auth(arbiter);
   check(choice == ARBIT_YES || choice == ARBIT_NO, "choice value invalid");

   //仲裁中/仲裁放币/仲裁取消
   int ret = update_art(arbiter, pair, deal_id, choice, reason);

   if (ret == 0)
      return;

   auto str_pair = pair.to_string();
   str_pair = lower_str(str_pair);
   deal_index_t dealtable_(_self, name{str_pair}.value);
   auto it = dealtable_.require_find(deal_id, "吃单找不到");

   //发送一个放币的延迟事物
   if (ret == 1)
   {

      name asker = it->side == MARKET_ORDER_SIDE_ASK ? it->user : it->maker_user;
      // ACTION cmddefcmdeal(name cmder, const symbol_code &pair, name asker, uint64_t deal_id, uint8_t status, const std::string &reason);
      transaction t{};
      t.actions.emplace_back(
          permission_level(arbiter, name{"active"}),
          _self,
          name{"cmddefcmdeal"},
          std::make_tuple(arbiter, pair, asker, deal_id, DEAL_STATUS_ARB_PLAYCOIN_FINISHED, "仲裁放币延迟事务"));

      auto itr_pair = get_market(pair);
      t.delay_sec = itr_pair->def_playcoin_time;
      t.send(it->arb_sender_id, _self);
      print("仲裁团仲裁结果是放币(卖方作为失败者，可以发起终审)发起一个延时放币事务ok，事务id=", it->arb_sender_id);

      send_trans_addblacklist(arbiter, asker, it, "卖方仲裁失败,必须在48h以内二次终审,否则进入黑名单");
   }
   else if (ret == 2)
   {

      transaction t{};
      t.actions.emplace_back(
          permission_level(arbiter, name{"active"}),
          _self,
          name{"defcldeal"},
          std::make_tuple(pair, arbiter, deal_id, DEAL_STATUS_ARB_CANCEL_FINISHED, "仲裁取消放币延迟事务"));

      auto itr_pair = get_market(pair);
      t.delay_sec = itr_pair->def_cancel_time;
      t.send(it->arb_sender_id, _self);
      print("仲裁团仲裁结果是不放币(买方作为失败者，可以发起终审)发起一个延时放币事务ok，事务id=", it->arb_sender_id);
      name bider = it->side == MARKET_ORDER_SIDE_BID ? it->user : it->maker_user;
      send_trans_addblacklist(arbiter, bider, it, "买方仲裁失败,必须在48h以内二次终审,否则进入黑名单");
   }
}

//申请终审 zz  push action otcexchange
//putjudge '["dabaicai","ask","ADXCNY",3,["18588250437","656717520@qq.com"],"买家其实没有付款","买家作弊",["http://zhongyingying.qicp.io:28080/ipfs/QmcnSL6oApFVYDb4wmUD5XLxxBzFW2H51f2Sdu3QjcFZqA"],["url"],"卖家发起终审"]' -p dabaicai
ACTION otcexchange::putjudge(name failer, const symbol_code &pair, uint64_t deal_id, const std::vector<std::string> &vec_contacts,
                             const std::string &reason,
                             const std::string &desc,
                             const std::vector<std::string> &vec_img,
                             const std::vector<std::string> &vec_video,
                             const std::string &source,
                             const std::string &email)
{
   require_auth(failer);
   auto str_pair = pair.to_string();
   str_pair = lower_str(str_pair);
   deal_index_t dealtable_(_self, name{str_pair}.value);
   auto it = dealtable_.require_find(deal_id, "吃单找不到");

   check(failer == it->user || failer == it->maker_user, "复审申请者不是deal参与方");

   bool ing = it->status == DEAL_STATUS_PAID_ARBIARATE_CANCEL || it->status == DEAL_STATUS_PAID_ARBIARATE_PALYCOIN; //在延迟事物还没有结束之前赶紧发起终审

   bool ed = it->status == DEAL_STATUS_ARB_CANCEL_FINISHED || it->status == DEAL_STATUS_ARB_PLAYCOIN_FINISHED; //就算完成了，还可以发起终审

   //bool self = it->status == DEAL_STATUS_PAID_PLAYCOIN_ING || it->status == DEAL_STATUS_SUCCESS_FINISHED; //卖方点击了主动放币，但是他也想发起终审

   check(ing || ed, "复审时deal的状态必须是仲裁结果执行中或者执行结束（48h以内)也可以发起终审");

   //终审只能由失败者发起，检验who是不是失败者
   auto now = CURRENT_SEC;

   arborder_index_t arbs(_self, name{str_pair}.value); //订单表
   auto it_order = arbs.require_find(deal_id, "仲裁order找不到");

   check((now.sec_since_epoch() - it_order->utime.sec_since_epoch()) <= DAY_SECONDS * 2, "必须48h之内发起终审");

   name asker = (it->side == MARKET_ORDER_SIDE_ASK) ? (it->user) : (it->maker_user);
   name bider = (it->side == MARKET_ORDER_SIDE_BID) ? (it->user) : (it->maker_user);

   name failer_name = (it_order->yes_num > it_order->no_num) ? asker : bider;
   uint8_t fail_side = (it_order->yes_num > it_order->no_num) ? MARKET_ORDER_SIDE_ASK : MARKET_ORDER_SIDE_BID;
   check(failer == failer_name, "终审只能由失败者发起");

   judge_index_t j(_self, name{str_pair}.value);
   auto itr_judge = j.find(deal_id);

   check(itr_judge == j.end(), failer_name.to_string().append("已经发起过终审了"));

   j.emplace(_self, [&](judge &ju) {
      ju.id = deal_id;          //作为主键 =deal_id
      ju.deal_id = deal_id;     //成交记录id
      ju.arborder_id = deal_id; //仲裁订单id
      ju.pair = pair;           //交易对
      ju.ctime = now;           //创建时间
      ju.utime = now;           //更新时间

      ju.judge_side = fail_side;   //终审者是买还是卖,只有失败方才能终审
      ju.bid_user = bider;         //买币的用户
      ju.ask_user = asker;         //卖币的用户
      ju.arb_failer = failer_name; //仲裁失败者,也就是终审发起方

      ju.content = ""; //申诉的内容

      //phone email reason desc
      ju.contacts = vec_contacts; //联系方式
      ju.reason = reason;         //申诉原因
      ju.desc = desc;             //问题描述
      ju.imgs = vec_img;
      ju.videos = vec_video;
      ju.source = source; //备注
      ju.s1 = email;      //备用
      ju.s2 = "";         //备用

      //新增一个申诉，这个时候要生成一个仲裁订单，然后这个订单下面又有很多仲裁log，来记录仲裁员的仲裁记录

      std::set<name> judgers; //分配终审员

      auto itr_pair = get_market(pair);
      get_avail_judger(itr_pair->stock.code(), judgers, GROUP_JUDPEOPLE_NUM, now, it);

      check(judgers.size() >= GROUP_JUDPEOPLE_MIN_NUM, "系统仲裁员个数<=1,无法分配终审任务");

      judgeorder_index_t t(_self, name{str_pair}.value);

      t.emplace(_self, [&](judgeorder &a) {
         a.id = ju.id;
         a.ctime = now;
         a.utime = now;
         a.pair = pair;
         a.judge_id = deal_id;
         a.deal_id = deal_id;
         a.initiator_side = fail_side;
         a.amount = it->amount;
         a.price = it->price;
         a.quota = it->quota;
         a.judge_fee = itr_pair->zero_stock;
         a.vec_judge = judgers;
         a.person_num = a.vec_judge.size();
         a.yes_num = 0;
         a.no_num = 0;
         a.status = APPEAL_STATUS_JUDGE_STARTED;
         a.status_str = APPEAL_STATUS_JUDGE_STARTED_STR;
         a.source = source;
      });

      std::map<name, uint64_t> map_person_task;
      std::map<name, uint8_t> map_person_pick;
      for (auto j : judgers)
      {
         judgetask_index_t jt(_self, j.value); //表名，按用户分表，不是按pair分表
         jt.emplace(_self, [&](judgetask &log) {
            log.id = jt.available_primary_key();
            log.side = fail_side;
            log.judge_time = now;
            log.ctime = now;
            log.utime = log.ctime;
            log.pair = pair;
            log.stock = itr_pair->stock.code();
            log.money = itr_pair->money.code();
            log.amount = it->amount;
            log.price = it->price;
            log.quota = it->quota;
            log.arborder_id = deal_id;
            log.judgeorder_id = deal_id;
            log.deal_id = deal_id;
            log.judge_id = deal_id;

            log.group_yes_num = 0;
            log.group_no_num = 0;

            log.self_judge_result = JUDGE_UNKOWN;
            log.group_judge_result = JUDGE_UNKOWN;
            log.labor_fee = itr_pair->zero_stock;
            log.status = JUDTASK_STATUS_CREATED;
            log.status_str = JUDTASK_STATUS_CREATED_STR;
            log.source = source;
            map_person_task.emplace(j, log.id);
            map_person_pick.emplace(j, JUDGE_UNKOWN);
         });

         //每个用户仲裁总数+1,然后每个仲裁员处于工作状态
         update_judger_total_num(itr_pair->stock.code(), j);
      }

      //写回
      auto itr_judorder = t.require_find(deal_id, "找不到仲裁订单表");
      t.modify(itr_judorder, _self, [&map_person_task, &map_person_pick](judgeorder &a) {
         a.map_person_task = map_person_task;
         a.map_person_pick = map_person_pick;
      });
   });

   //取消仲裁者的延迟事物,是否仲裁被执行了
   bool has_judge_arbcmd = false; //延迟事物还在执行中，先取消
   if (ing)
   {
      has_judge_arbcmd = cancel_deftransaction(it->arb_sender_id, "发起了终审，取消仲裁发布的延迟事物，等待终审结果");
   }

   arbs.modify(it_order, _self, [&](arborder &a) {
      a.utime = now;
      a.has_judge_arbcmd = has_judge_arbcmd;
      a.status = APPEAL_STATUS_JUDGE_STARTED;
      a.status_str = APPEAL_STATUS_JUDGE_STARTED_STR;
   });

   update_dh_status(it->user, it->taker_id, DEAL_STATUS_PAID_JUDGING);
   update_dh_status(it->maker_user, it->maker_id, DEAL_STATUS_PAID_JUDGING);

   std::string str_cancel{",因为失败方已经发起二次终审,所以要阻止其48h进入小黑屋"};

   bool cancel_trans_blacklist = cancel_deftransaction(it->arbfailer_balacklist_sender_id, str_cancel);
   if (cancel_trans_blacklist)
   {
      str_cancel.append(",取消小黑屋成功");
   }
   else
   {
      str_cancel.append(",取消小黑屋失败");
   }

   dealtable_.modify(it, _self, [&](deal &d) {
      d.utime = current_time_point();
      d.status = DEAL_STATUS_PAID_JUDGING;         //终审中
      d.status_str = DEAL_STATUS_PAID_JUDGING_STR; //终审中
      d.source.append("|").append(failer.to_string()).append("发起终审:").append(DEAL_STATUS_PAID_JUDGING_STR).append(str_cancel);
   });

   //取消失败，不在表里  取消成功，还在表里
   user_deal_not_fill(it->user, source);
}

ACTION otcexchange::judgedeal(name judger, const symbol_code &pair, uint64_t deal_id, uint8_t choice, const std::string &reason)
{
   require_auth(judger);
   check(choice == JUDGE_NO || choice == JUDGE_YES, "judge choice invalid");

   auto str_pair = pair.to_string();
   str_pair = lower_str(str_pair);
   auto pair_scope = name{str_pair}.value;
   deal_index_t dealtable_(_self, pair_scope);
   auto it = dealtable_.require_find(deal_id, "吃单找不到");

   check(it->status == DEAL_STATUS_PAID_JUDGING, "只有处于终审中的deal才能执行终审");

   judgeorder_index_t jorder(_self, pair_scope);
   auto it_jorder = jorder.require_find(deal_id, "终审order找不到");

   auto itrtask = it_jorder->map_person_task.find(judger);
   check(itrtask != it_jorder->map_person_task.end(), "该终审员还没有分配终审任务");

   auto taskid = itrtask->second;
   judgetask_index_t jt(_self, judger.value);
   auto it_task = jt.require_find(taskid, "该终审员的task找不到");
   check(it_task->status == JUDTASK_STATUS_CREATED, "终审订单不是已创建,有可能被系统取消或者已经终审过了");

   auto itr_pair = get_market(pair);
   rudece_judger_work_num(itr_pair->stock.code(), judger);

   auto now = CURRENT_SEC;

   /**说明已经有其他终审者率先作出终审
   if (it_jorder->status != APPEAL_STATUS_JUDGE_STARTED)
   {
      const std::string source("，您好，请知悉：终审结果已经出来，你的终审结果提交太迟了，下次请快点，你的终审task的状态将被设定为已被系统取消");

      jt.modify(it_task, _self, [&](judgetask &t) {
         t.utime = now;
         t.self_judge_result = choice;
         t.status = JUDTASK_STATUS_CANCELED;
         t.status_str = JUDTASK_STATUS_CANCELED_STR;
         t.source.append(reason).append("|").append(source);
         t.group_judge_result = (it_jorder->yes_num > it_jorder->no_num) ? JUDGE_YES : JUDGE_NO;
         t.group_yes_num = it_jorder->yes_num;
         t.group_no_num = it_jorder->no_num;
      });

      return;
   }
   * */

   uint8_t judge_status = JUDGE_UNKOWN;
   std::string judge_status_str("");

   uint8_t judge_ostatus = JUDGE_UNKOWN;
   std::string judge_ostatus_str("");

   uint8_t yes_num = 0;
   uint8_t no_num = 0;

   if (choice == JUDGE_YES)
   {
      judge_status = JUDTASK_STATUS_YES;
      judge_status_str = JUDTASK_STATUS_YES_STR;
      yes_num = 1;
      judge_ostatus = APPEAL_STATUS_JUDGE_YES;
      judge_ostatus_str = APPEAL_STATUS_JUDGE_YES_STR;
   }
   else
   {
      judge_status = JUDTASK_STATUS_NO;
      judge_status_str = JUDTASK_STATUS_NO_STR;
      no_num = 1;

      judge_ostatus = APPEAL_STATUS_JUDGE_NO;
      judge_ostatus_str = APPEAL_STATUS_JUDGE_NO_STR;
   }

   //更新自己的终审任务
   jt.modify(it_task, _self, [&](judgetask &t) {
      t.utime = now;
      t.self_judge_result = choice;
      t.status = judge_status;
      t.status_str = judge_status_str;
      t.source.append(reason);
      t.group_judge_result = choice;
      t.group_yes_num = yes_num;
      t.group_no_num = no_num;
   });
   //更新终身订单
   jorder.modify(it_jorder, _self, [&](judgeorder &j) {
      j.utime = now;
      j.yes_num = yes_num;
      j.no_num = no_num;
      j.status = judge_ostatus;
      j.status_str = judge_ostatus_str;
      j.source.append("|").append(reason);
      j.map_person_pick[judger] = choice;
   });

   //对于其他任务而言
   auto map = it_jorder->map_person_task;
   for (auto item : map)
   {
      if (item.first != judger)
      {
         auto taskid = item.second;
         judgetask_index_t jt(_self, item.first.value);
         auto it_task = jt.require_find(taskid, "该终审员的task找不到");
         rudece_judger_work_num(itr_pair->stock.code(), item.first);

         const std::string source("，您好，请知悉：终审结果已经出来，你的终审结果提交太迟了，下次请快点，你的终审task的状态将被设定为已被系统取消");

         jt.modify(it_task, _self, [&](judgetask &t) {
            t.utime = now;
            t.self_judge_result = JUDGE_UNKOWN;
            t.status = JUDTASK_STATUS_CANCELED;
            t.status_str = JUDTASK_STATUS_CANCELED_STR;
            t.source.append(reason).append("|").append(source);
            t.group_judge_result = choice;
            t.group_yes_num = yes_num;
            t.group_no_num = no_num;
         });
      }
   }

   //更新仲裁订单表
   arborder_index_t arbs(_self, pair_scope); //订单表
   auto it_order = arbs.require_find(deal_id, "仲裁order找不到");

   name asker = (it->side == MARKET_ORDER_SIDE_ASK) ? (it->user) : (it->maker_user);
   name bider = (it->side == MARKET_ORDER_SIDE_BID) ? (it->user) : (it->maker_user);
   bool isyes = (it_order->yes_num > it_order->no_num); //是否是放币

   name failer_name = isyes ? asker : bider; //仲裁的失败方

   bool has_judge_arbcmd = it_order->has_judge_arbcmd;

   //找到每个仲裁者的情况
   auto map_picks = it_order->map_person_pick; //决出胜利时的仲裁者的选择

   std::set<name> reparatio_arbers;
   std::set<name> fail_to_success_arbers;

   name judge_failer = name{"unkown"};
   name judge_successer = name{"unkown"};

   if (isyes) //仲裁放币,卖家是失败方，发起二次终审
   {
      if (choice == JUDGE_YES) //终审放币
      {
         judge_failer = asker;
         judge_successer = bider;
         if (it_order->has_judge_arbcmd)
         {
            //deal最新的状态
            std::string msg{"卖币者恶意终审,但是仲裁放币动作被取消，再次执行放币,这时把这个卖币者加入黑名单"};
            print(msg);
            commit_deal(asker, dealtable_, it, DEAL_STATUS_JUD_PLAYCOIN_FINISHED, msg);
         }
         else
         {
            print("卖币者恶意终审,但是仲裁放币已经执行完成,这时把用户加入黑名单即可");
         }
      }
      else if (choice == JUDGE_NO) //卖家申诉成功了。
      {
         judge_failer = bider;
         judge_successer = asker;
         //终审推翻了仲裁，但是仲裁的结果是否执行？
         if (it_order->has_judge_arbcmd) //仲裁的结果是放币，且放币已经完成，这个时候要补偿卖方
         {
            print("卖币者终审成功,但是仲裁放币已经被取消,先1.执行取消放币，2.在弥补卖方 3.恶意仲裁者进小黑屋");
            rollback_deal(dealtable_, it, DEAL_STATUS_JUD_CANCEL_FINISHED, reason);
         }
         else //仲裁的结果是放币，且放币没有完成被取消，这个时候要补偿卖方
         {
            print("卖币者终审成功,但是仲裁放币已经被执行了,1.仲裁者要补充这个卖币者，2.恶意仲裁者进小黑屋");
            //todo
         }

         //终审推翻了,仲裁放币的人要赔偿
         for (auto &item : map_picks)
         {
            if (item.second == ARBIT_YES)
            {
               reparatio_arbers.emplace(item.first);
            }
            if (item.second == ARBIT_NO)
            {
               fail_to_success_arbers.emplace(item.first);
            }
         }
      }
   }
   else //仲裁取消，买家发起二次仲裁
   {
      if (choice == JUDGE_NO)
      {
         judge_failer = bider;
         judge_successer = asker;
         if (it_order->has_judge_arbcmd)
         {
            print("买币者恶意终审,仲裁取消放币已经被取消,1.执行取消放币 2.买方加黑名单");
            rollback_deal(dealtable_, it, DEAL_STATUS_JUD_CANCEL_FINISHED, reason);
         }
         else
         {
            print("买币者恶意终审,仲裁取消放币已经执行完,1.买方加黑名单");
         }
      }
      else if (choice == JUDGE_YES)
      {
         judge_failer = asker;
         judge_successer = bider;
         if (it_order->has_judge_arbcmd)
         {
            print("买币者终审成功,但是仲裁取消放币已经被取消,1.放币 2.仲裁者补偿买方10% 3.恶意仲裁者进小黑屋");
            commit_deal(asker, dealtable_, it, DEAL_STATUS_JUD_PLAYCOIN_FINISHED, "");
         }
         else
         {
            print("买币者终审成功,但是仲裁取消放币已经被执行,1.仲裁者补偿买方150% 2.恶意仲裁者进小黑屋");
         }

         for (auto &item : map_picks)
         {
            if (item.second == ARBIT_NO)
            {
               reparatio_arbers.emplace(item.first);
            }
            if (item.second == ARBIT_YES)
            {
               fail_to_success_arbers.emplace(item.first);
            }
         }
      }
   }

   auto arb_status = choice == JUDGE_NO ? APPEAL_STATUS_JUDGE_NO : APPEAL_STATUS_JUDGE_YES;
   auto arb_status_str = choice == JUDGE_NO ? APPEAL_STATUS_JUDGE_NO_STR : APPEAL_STATUS_JUDGE_YES_STR;

   arbs.modify(it_order, _self, [&](arborder &a) {
      a.judger = judger;
      a.utime = CURRENT_SEC;
      a.status = arb_status;
      a.status_str = arb_status_str;
      a.judge_result = choice;
   });

   uint8_t status = choice == JUDGE_NO ? DEAL_STATUS_JUD_CANCEL_FINISHED : DEAL_STATUS_JUD_PLAYCOIN_FINISHED;
   std::string status_str = choice == JUDGE_NO ? DEAL_STATUS_JUD_CANCEL_FINISHED_STR : DEAL_STATUS_JUD_PLAYCOIN_FINISHED_STR;

   update_dh_status(it->user, it->taker_id, status);
   update_dh_status(it->maker_user, it->maker_id, status);

   //立即更改状态使用户看的到,deal处于终审放币或者终审放币取消
   dealtable_.modify(it, _self, [&](deal &d) {
      d.utime = time_point_sec(current_time_point().sec_since_epoch());
      d.status = status;
      d.status_str = status_str;
      d.source.append("|终审员").append(judger.to_string()).append(" ").append(reason).append("作出终审:").append(status_str);
      d.source.append(reason).append(",终审失败者").append(judge_failer.to_string()).append("进小黑屋");
      d.judge_failer = judge_failer;
   });

   user_deal_fill(it->user);

   send_action_addblacklist(judger, judge_failer, it, "终审失败者");

   //失败的仲裁员要补偿申诉成功的人
   if (!reparatio_arbers.empty())
   {
      asset reparate_amount = it_order->has_judge_arbcmd ? cal_asset(it->amount, 10) : cal_asset(it->amount, 150);
      //怎么分配呢？
      asset avail_amount = ZERO_ASSET(it->amount.symbol);

      std::string str_stock = itr_pair->stock.code().to_string();
      str_stock = lower_str(str_stock);
      arbiter_index_t t(_self, name{str_stock}.value);

      for (auto &item : reparatio_arbers)
      {
         auto itr_arbiter = t.require_find(item.value, "judgedeal:仲裁员表找不到此仲裁员");
         avail_amount += itr_arbiter->balance;
      }

      uint32_t ratio = static_cast<uint32_t>((reparate_amount / avail_amount) * 100 + 0.5);

      for (auto &item : reparatio_arbers)
      {

         auto itr_arbiter = t.require_find(item.value, "judgedeal:仲裁员表找不到此仲裁员");
         asset pay = cal_asset(itr_arbiter->balance, ratio);
         if (pay > itr_arbiter->balance)
         {
            pay = itr_arbiter->balance;
         }

         settle_stock(item, judge_successer, pay, ZERO_ASSET(it->amount.symbol), "终审成功，仲裁员补充仲裁失败方钱", true);

         //更新一下仲裁员的抵押的钱和状态,也就是清退
         t.modify(itr_arbiter, _self, [&](arbiter &a) {
            a.balance = a.balance - pay;
            a.utime = CURRENT_SEC;
            a.status = ARBUSER_STATUS_BALACKLIST;
            a.last_arb_time = a.utime;
         });

         send_action_addblacklist(judger, item, it, "仲裁者恶意仲裁");

         //更改自己的角色
         auto current_role = get_userrole(item, itr_pair->stock.code());
         if (current_role == USER_ROLE_ARB)
         {
            insert_modify_userrole(item, itr_pair->stock.code(), USER_ROLE_ARB_BALACKLIST);
         }
         else if (current_role == USER_ROLE_EX_ARB)
         {
            insert_modify_userrole(item, itr_pair->stock.code(), USER_ROLE_EX_ARB_BALACKLIST);
         }

         //更改仲裁员的胜利场数
         update_arbiter_win_num(itr_pair->stock.code(), item, -1);
      }

      asset total_fee = (it->side == MARKET_ORDER_SIDE_ASK) ? it->bid_fee : it->ask_fee;
      put_arbpool_fee(itr_pair->stock, now, std::set<name>{}, -(total_fee / 2), it);
   }
   for (auto &item : fail_to_success_arbers)
   {
      update_arbiter_win_num(itr_pair->stock.code(), item, 1);
   }
}

ACTION otcexchange::moddealss(const symbol_code &pair, uint64_t deal_id, uint8_t status, const std::string &reason)
{
   require_auth(_self);
   auto str_pair = pair.to_string();
   str_pair = lower_str(str_pair);
   deal_index_t dealtable_(_self, name{str_pair}.value);
   auto it = dealtable_.require_find(deal_id, "吃单找不到");
   dealtable_.modify(it, _self, [&](deal &d) {
      d.status = status;
      d.status_str = get_deal_status_str(status);
      d.source.append("|合约修改").append(reason).append(d.status_str);
   });
}

void otcexchange::update_stock_other_base_price_role(const symbol_code &pair)
{
   auto itr_pair = markets_.require_find(pair.raw(), ERR_MSG_CHECK_FAILED(50110, pair.to_string().append(ERR_MSG_PAIR_NOT_EXIST)).c_str());
   auto pairs = markets_.get_index<"bystock"_n>();
   auto beg = pairs.lower_bound(itr_pair->stock.raw());
   auto end = pairs.upper_bound(itr_pair->stock.raw());
   while (beg != end)
   {
      print(beg->pair.to_string());
      if (beg->pair != pair && beg->base_price_role == 1)

         markets_.modify(*beg, _self, [&](market &m) {
            m.base_price_role = 2;
            m.utime = CURRENT_SEC;
         });

      beg++;
   }
}

void otcexchange::get_avail_arbiter(const symbol_code &stock, std::set<name> &res, int num, const time_point_sec &ctime, deal_iter_t itr_deal)
{
   //在线时间优先
   std::string str_stock = stock.to_string();
   str_stock = lower_str(str_stock);
   arbiter_index_t arbiters_(_self, name{str_stock}.value);

   auto atime = arbiters_.get_index<"bytime"_n>();
   auto cc = ctime.sec_since_epoch() % DAY_SECONDS;
   auto pos = atime.lower_bound(0);
   auto end = atime.upper_bound(cc + 1);

   //满足在线时间的
   std::set<name> time_user;
   //即满足在线时间又没有工作的用户
   std::set<name> notwork_user;
   //满足时间没有工作钱够
   std::set<name> money_enough_user;

   while (pos != end)
   {
      bool notexuser = pos->account != itr_deal->user && pos->account != itr_deal->maker_user;

      if (notexuser && pos->online_beg_time <= cc && pos->online_end_time >= cc && pos->status == ARBUSER_STATUS_REGED)
      {
         time_user.insert(pos->account);
         bool isnotwork = arbiter_isnotwork(*pos);
         if (isnotwork)
         {
            notwork_user.insert(pos->account);
         }

         if (isnotwork && arbiter_isenough(*pos, itr_deal->amount))
         {
            if (money_enough_user.size() < num)
            {
               money_enough_user.insert(pos->account);
            }
            else
            {
               break;
            }
         }
      }
      ++pos;
   }

   for (auto &item : money_enough_user)
   {
      res.insert(item);
   }

   for (auto &item : notwork_user)
   {
      if (res.size() < num)
      {
         res.insert(item);
      }
      else
      {
         break;
      }
   }

   for (auto &item : time_user)
   {
      if (res.size() < num)
      {
         res.insert(item);
      }
      else
      {
         break;
      }
   }

   //还没找够怎么办

   if (res.size() < num)
   {
      auto awinnum = arbiters_.get_index<"bywinnum"_n>();
      auto it_winnum = awinnum.rbegin();

      while (it_winnum != awinnum.rend() && res.size() < num)
      {
         bool notexuser = it_winnum->account != itr_deal->user && it_winnum->account != itr_deal->maker_user;
         if (notexuser && it_winnum->status == ARBUSER_STATUS_REGED)
         {
            res.insert(it_winnum->account);
         }
         ++it_winnum;
      }
   }

   if (res.size() < num)
   {
      auto awinrate = arbiters_.get_index<"bywinrate"_n>();
      auto it_winrate = awinrate.rbegin();
      while (it_winrate != awinrate.rend() && res.size() < num)
      {
         bool notexuser = it_winrate->account != itr_deal->user && it_winrate->account != itr_deal->maker_user;
         if (notexuser && it_winrate->status == ARBUSER_STATUS_REGED)
         {
            res.insert(it_winrate->account);
         }
         ++it_winrate;
      }
   }
}

void otcexchange::get_avail_judger(const symbol_code &stock, std::set<name> &res, int num, const time_point_sec &ctime, deal_iter_t itr_deal)
{
   //在线时间优先
   std::string str_stock = stock.to_string();
   str_stock = lower_str(str_stock);
   judger_index_t t_(_self, name{str_stock}.value);

   auto atime = t_.get_index<"bytime"_n>();
   auto cc = ctime.sec_since_epoch() % DAY_SECONDS;
   auto pos = atime.lower_bound(0);
   auto end = atime.upper_bound(cc + 1);

   //满足在线时间的
   std::set<name> time_user;
   //即满足在线时间又没有工作的用户
   std::set<name> notwork_user;

   while (pos != end)
   {
      bool notexuser = pos->account != itr_deal->user && pos->account != itr_deal->maker_user;

      if (notexuser && pos->online_beg_time <= cc && pos->online_end_time >= cc && pos->status == JUDUSER_STATUS_REGED)
      {
         time_user.insert(pos->account);
         bool isnotwork = judger_isnotwork(*pos);
         if (isnotwork)
         {
            if (notwork_user.size() < num)
            {
               notwork_user.insert(pos->account);
            }
            else
            {
               break;
            }
         }
      }
      ++pos;
   }

   for (auto &item : notwork_user)
   {
      if (res.size() < num)
      {
         res.insert(item);
      }
      else
      {
         break;
      }
   }

   for (auto &item : time_user)
   {
      if (res.size() < num)
      {
         res.insert(item);
      }
      else
      {
         break;
      }
   }

   //还没找够怎么办

   if (res.size() < num)
   {
      auto beg = t_.begin();
      auto end = t_.end();

      while (beg != end && res.size() < num)
      {
         bool notexuser = beg->account != itr_deal->user && beg->account != itr_deal->maker_user;
         if (notexuser && beg->status == JUDUSER_STATUS_REGED)
         {
            res.insert(beg->account);
         }
         ++beg;
      }
   }
}

int otcexchange::update_art(name arbiter, const symbol_code &pair, uint64_t deal_id, uint8_t choice, const std::string &reason)
{

   int ret = 0;
   auto str_pair = pair.to_string();
   str_pair = lower_str(str_pair);
   deal_index_t dealtable_(_self, name{str_pair}.value);
   auto it = dealtable_.require_find(deal_id, "吃单找不到");

   //处于仲裁状态，正式环境是分配9个人，3个人结果一直就结束仲裁
   check(it->status == DEAL_STATUS_PAID_APPEAL_ASK ||
             DEAL_STATUS_PAID_APPEAL_BID ||
             DEAL_STATUS_PAID_APPEAL_ALL ||
             DEAL_STATUS_PAID_ARBIARATE_ING,
         "deal必须处于申诉状态和仲裁中才能发起仲裁,仲裁结果已出，终止仲裁");

   arborder_index_t arbs(_self, name{str_pair}.value); //订单表
   auto it_order = arbs.require_find(deal_id, "仲裁order找不到");

   auto itt = it_order->map_person_task.find(arbiter);
   check(itt != it_order->map_person_task.end(), "该仲裁员还没有分配仲裁任务");
   auto taskid = itt->second;

   arbtask_index_t arbtasks(_self, arbiter.value); //表名，按用户分表，不是按pair分表
   auto it_task = arbtasks.require_find(taskid, "该仲裁员仲裁task找不到");
   check(it_task->status == ARBTASK_STATUS_CREATED, "已经仲裁过了,不能重复仲裁或者被已提前达成共识被系统取消");

   auto itr_pair = get_market(pair);
   rudece_arbiter_work_num(itr_pair->stock.code(), arbiter); //已经仲裁完成，正在进行中的仲裁任务减1

   auto yes_num = it_order->yes_num;
   auto no_num = it_order->no_num;
   auto now = CURRENT_SEC;

   /*
   if (yes_num >= GROUP_OVER_NUM || no_num >= GROUP_OVER_NUM) //如果仲裁结果已经出来了，就不在需要往下走了
   {
      const std::string source("，您好，请知悉：仲裁结果已经出来，你的仲裁结果提交太迟了，下次请快点，你的仲裁task的状态将被设定为已被系统取消");
      print(arbiter, source.c_str());

      arbtasks.modify(it_task, _self, [&](arbtask &t) {
         t.utime = now;
         t.self_arbit_result = choice;
         t.status = ARBTASK_STATUS_CANCELED;
         t.status_str = ARBTASK_STATUS_CANCELED_STR;
         t.source.append(reason).append("|").append(source);

         t.group_arbit_result = (yes_num > no_num) ? ARBIT_YES : ARBIT_NO;
         t.group_yes_num = yes_num;
         t.group_no_num = no_num;
      });

      return ret;
   }
*/

   if (it->status != DEAL_STATUS_PAID_ARBIARATE_ING)
   {
      dealtable_.modify(it, _self, [&](deal &d) {
         d.utime = current_time_point();
         d.status = DEAL_STATUS_PAID_ARBIARATE_ING;
         d.status_str = DEAL_STATUS_PAID_ARBIARATE_ING_STR;
         d.source.append("|第一个仲裁员").append(arbiter.to_string()).append("仲裁:").append(DEAL_STATUS_PAID_ARBIARATE_ING_STR);
      });

      update_dh_status(it->user, it->taker_id, DEAL_STATUS_PAID_ARBIARATE_ING);
      update_dh_status(it->maker_user, it->maker_id, DEAL_STATUS_PAID_ARBIARATE_ING);
   }

   if (choice == ARBIT_YES)
   {
      yes_num++;
   }
   else
   {
      no_num++;
   }

   auto yesorno = yes_num > no_num ? true : false;

   if (yesorno)
   {

      if (yes_num >= GROUP_OVER_NUM)
      {
         ret = 1;
      }
   }
   else
   {
      if (no_num >= GROUP_OVER_NUM)
      {
         ret = 2;
      }
   }

   name asker = (it->side == MARKET_ORDER_SIDE_ASK) ? it->user : it->maker_user;
   name bider = (it->side == MARKET_ORDER_SIDE_ASK) ? it->maker_user : it->user;

   uint8_t group_result = ARBIT_UNKOWN;

   auto arborder_status = APPEAL_STATUS_ARB_ING;
   auto arborder_status_str = APPEAL_STATUS_ARB_ING_STR;

   auto deal_status = DEAL_STATUS_PAID_ARBIARATE_ING;
   auto deal_status_str = DEAL_STATUS_PAID_ARBIARATE_ING_STR;

   bool my_win = false; //还不知道我是否赢

   std::string detail{""};

   if (ret == 1)
   {
      group_result = ARBIT_YES;
      arborder_status = APPEAL_STATUS_ARB_YES;
      arborder_status_str = APPEAL_STATUS_ARB_YES_STR;
      if (choice == ARBIT_YES)
      {
         my_win = true;
      }
      deal_status = DEAL_STATUS_PAID_ARBIARATE_PALYCOIN;
      deal_status_str = DEAL_STATUS_PAID_ARBIARATE_PALYCOIN_STR;

      update_dh_status(it->user, it->taker_id, DEAL_STATUS_PAID_ARBIARATE_PALYCOIN);
      update_dh_status(it->maker_user, it->maker_id, DEAL_STATUS_PAID_ARBIARATE_PALYCOIN);

      detail.append("仲裁团结果是卖方放币,卖方");
      detail.append(asker.to_string()).append(" 失败，dealid：").append(std::to_string(deal_id));

      update_userdaystat(asker, now, ARB, 1, detail);
   }
   else if (ret == 2)
   {
      group_result = ARBIT_NO;
      arborder_status = APPEAL_STATUS_ARB_NO;
      arborder_status_str = APPEAL_STATUS_ARB_NO_STR;
      if (choice == ARBIT_NO)
      {
         my_win = true;
      }
      deal_status = DEAL_STATUS_PAID_ARBIARATE_CANCEL;
      deal_status_str = DEAL_STATUS_PAID_ARBIARATE_CANCEL_STR;

      update_dh_status(it->user, it->taker_id, DEAL_STATUS_PAID_ARBIARATE_CANCEL);
      update_dh_status(it->maker_user, it->maker_id, DEAL_STATUS_PAID_ARBIARATE_CANCEL);

      detail.append("仲裁团结果是取消放币,买方");
      detail.append(bider.to_string()).append(" 失败，dealid：").append(std::to_string(deal_id));

      update_userdaystat(bider, now, ARB, 1, reason);
   }

   //更新arborder
   arbs.modify(it_order, _self, [&](arborder &o) {
      o.utime = now;
      o.yes_num = yes_num;
      o.no_num = no_num;
      o.map_person_pick[arbiter] = choice;
      o.status = arborder_status;
      o.status_str = arborder_status_str;
   });

   //更新arbtask
   auto ts = choice == ARBIT_NO ? ARBTASK_STATUS_NO : ARBTASK_STATUS_YES;
   auto ts_str = choice == ARBIT_NO ? ARBTASK_STATUS_NO_STR : ARBTASK_STATUS_YES_STR;

   arbtasks.modify(it_task, _self, [&](arbtask &t) {
      t.utime = now;
      t.self_arbit_result = choice; //我自己说NO
      t.status = ts;
      t.status_str = ts_str;
      t.group_arbit_result = group_result;
      t.source = reason;
      t.group_yes_num = yes_num;
      t.group_no_num = no_num;
   });

   auto map_task = it_order->map_person_task;
   std::set<name> win_arbers;
   for (auto &item : map_task)
   {
      if (item.first != arbiter) //更新我的兄弟共有的东西
      {
         arbtask_index_t tasks(_self, item.first.value); //表名，按用户分表，不是按pair分表
         auto itr_task = tasks.require_find(item.second, "该仲裁员仲裁task找不到");

         //仲裁结果出来了且 已作出选择的兄弟
         if ((itr_task->self_arbit_result == ARBIT_YES && ret == 1) || (itr_task->self_arbit_result == ARBIT_NO && ret == 2))
         {
            update_arbiter_win_num(itr_pair->stock.code(), item.first, 1);
            win_arbers.emplace(item.first);
         }
         //仲裁结果出来了且没来得及仲裁的弟兄
         if (ret != 0 && itr_task->self_arbit_result == ARBIT_UNKOWN)
         {
            rudece_arbiter_work_num(itr_pair->stock.code(), item.first);
         }

         tasks.modify(itr_task, _self, [&itr_task, &itr_pair, &item, &ret, &group_result, &yes_num, &no_num, &now](arbtask &t) {
            t.utime = now;
            t.group_yes_num = yes_num;
            t.group_no_num = no_num;
            if (ret != 0) //中擦结果出来了才更新
            {
               t.group_arbit_result = group_result; //告诉所有的兄弟
            }
            //仲裁结果出来了且 没来得及仲裁的弟兄
            if (ret != 0 && itr_task->self_arbit_result == ARBIT_UNKOWN)
            {
               static const std::string source("，您好，请知悉：仲裁结果已经出来，你的仲裁task的状态将被设定为已被系统取消");
               t.status = ARBTASK_STATUS_CANCELED;
               t.status_str = ARBTASK_STATUS_CANCELED_STR;
            }
         });
      }
   }
   if (my_win)
   {
      update_arbiter_win_num(itr_pair->stock.code(), arbiter, 1);
      win_arbers.emplace(arbiter);
   }

   asset total_fee = (it->side == MARKET_ORDER_SIDE_ASK) ? it->bid_fee : it->ask_fee;
   put_arbpool_fee(itr_pair->stock, now, win_arbers, total_fee / 2, it);

   //更新deal的数据
   dealtable_.modify(it, _self, [&](deal &d) {
      d.utime = current_time_point();
      if (ret != 0)
      {
         d.status = deal_status; //立即更改状态使用户看的到，仲裁放币
         d.status_str = deal_status_str;
         d.arb_sender_id = get_avail_senderid(detail, "创建一个延迟放币/取消放币延迟事务id");
         d.arbfailer_balacklist_sender_id = get_avail_senderid(detail, "创建仲裁失败方禁止单日下单,48h没有二次终审将进小黑屋延迟事务id");
      }

      if (ret == 0)
      {
         d.source.append("|").append(arbiter.to_string()).append("仲裁结果:").append(std::to_string(choice)).append(APPEAL_STATUS_ARB_ING_STR);
      }
      else if (ret == 1)
      {
         d.arb_failer = asker;
         d.source.append("|共识放币:").append(arbiter.to_string()).append("仲裁,同时失败方禁止单日下单,48h没有二次终审将进小黑屋:").append(DEAL_STATUS_PAID_ARBIARATE_PALYCOIN_STR);
      }
      else if (ret == 2)
      {
         d.arb_failer = bider;
         d.source.append("|共识取消放币:").append(arbiter.to_string()).append("仲裁,同时失败方禁止单日下单,48h没有二次终审将进小黑屋:").append(DEAL_STATUS_PAID_ARBIARATE_CANCEL_STR);
      }
   });

   return ret;
}

void otcexchange::rollback_deal(deal_index_t &dealtable_, deal_iter_t itr_deal, uint8_t status, const std::string &reason)
{
   auto itr_pair = get_open_market(itr_deal->pair);
   auto user = itr_deal->user;          //deal的用户
   auto mk_user = itr_deal->maker_user; //广告主
   std::string mk_side = (itr_deal->side == MARKET_ORDER_SIDE_ASK) ? MARKET_ORDER_SIDE_BID_STR : MARKET_ORDER_SIDE_ASK_STR;
   auto adtable_ = get_adtable(itr_deal->pair, mk_side);
   auto itr_ad = adtable_.require_find(itr_deal->maker_order_id, "对应的广告订单不存在,无法取消");

   //deal已经取消了,回滚广告单的处理
   uint8_t new_ad_status = 0;
   adtable_.modify(itr_ad, _self, [&](adorder &ad) {
      ad.left = ad.left + itr_deal->amount;
      ad.freeze = ad.freeze - itr_deal->amount;
      ad.update_ad_status(itr_pair);
      new_ad_status = ad.status;
      //update_ad_status(ad, itr_pair);
   });
   if (new_ad_status == AD_STATUS_FINISHED)
   {
      update_user_ad_unfill_num(mk_user, -1);
   }

   //deal是买币的,就不用做什么操作了.
   //deal 表
   update_dh_status(itr_deal->user, itr_deal->taker_id, status);
   update_dh_status(itr_deal->maker_user, itr_deal->maker_id, status);

   dealtable_.modify(itr_deal, _self, [&](deal &d) {
      d.status = status;
      d.status_str = get_deal_status_str(status);
      d.utime = current_time_point();
      d.source.append("|").append(reason).append(d.status_str);

      //手动取消这些是不是重置为0
      //d.quota=itr_pair->zero_money;
      //d.ask_fee = itr_pair->zero_stock;
      //d.bid_fee = itr_pair->zero_stock;
   });
   //deal如果是taker卖币，还需要解冻,如果是广告主是卖币
   if (itr_deal->side == MARKET_ORDER_SIDE_ASK)
   {
      unfreeze_stock(name{TOKEN_TEMP_ACCOUNT}, itr_deal->user, itr_deal->amount, USER_TYPE_OTC, reason);

      std::string detail(user.to_string());
      detail.append("canceltk ask,deal_id :").append(std::to_string(itr_deal->id)).append(",ad_id:").append(std::to_string(itr_deal->maker_order_id)).append(",");
      detail.append("amount:").append(itr_deal->amount.to_string()).append(",");
      detail.append("price:").append(itr_deal->price.to_string()).append(",");
      detail.append("total freeze:").append(itr_deal->amount.to_string()).append("");
      inset_balancelog(user, itr_pair->stock.code(), BALANCE_MOD_CANCEL_TK, itr_deal->amount, itr_pair->zero_stock, itr_deal->amount, itr_pair->zero_stock, detail);
   }
   user_deal_fill(itr_deal->user);
}

void otcexchange::commit_deal(name asker, deal_index_t &dealtable_, deal_iter_t itr_deal, uint8_t status, const std::string &reason)
{
   auto user = itr_deal->user;
   name from;
   name to;
   bool is_bid_fee = false;

   if (itr_deal->side == MARKET_ORDER_SIDE_ASK) //是卖币是taker
   {
      /* code */
      from = itr_deal->user;
      to = itr_deal->maker_user;
      is_bid_fee = true;
   }
   else
   {
      to = itr_deal->user;
      from = itr_deal->maker_user;
      is_bid_fee = false;
   }
   check(asker == from, "放币的人搞错了");
   //校验deal的状态为放币中

   std::string str_mk_side = (itr_deal->side == MARKET_ORDER_SIDE_ASK) ? MARKET_ORDER_SIDE_BID_STR : MARKET_ORDER_SIDE_ASK_STR;
   auto adtable = get_adtable(itr_deal->pair, str_mk_side);
   auto itr_ad = adtable.require_find(itr_deal->maker_order_id, "ad order not exist");

   // 现在扣广告方的钱
   asset fee = (itr_deal->side == MARKET_ORDER_SIDE_ASK) ? itr_deal->bid_fee : itr_deal->ask_fee;

   if (itr_deal->side == MARKET_ORDER_SIDE_ASK)
   {
      auto itr_pair = get_market(itr_deal->pair);
      //广告主是买币方，广告主要付手续费，amount=100 fee=2 广告主实际得币98币
      settle_stock(from, to, itr_deal->amount, fee, "放币啦，卖币方把币打给买币的人，同时卖币方支付一笔手续费", is_bid_fee);

      std::string to_detail(from.to_string());
      to_detail.append("(taker)放币给").append(to.to_string()).append(" (广告主)");
      to_detail.append("deal_id :").append(std::to_string(itr_deal->id)).append(",ad_id:").append(std::to_string(itr_deal->maker_order_id)).append(",");
      to_detail.append("amount:").append(itr_deal->amount.to_string()).append(",");
      to_detail.append("price:").append(itr_deal->price.to_string()).append(",");
      to_detail.append("fee:").append(fee.to_string()).append(",");
      to_detail.append("广告主实际收到（amount-fee）:").append((itr_deal->amount - fee).to_string()).append(",");

      inset_balancelog(to, itr_pair->stock.code(), BALANCE_MOD_DEAL_ASK, itr_deal->amount, fee, itr_deal->amount - fee, itr_pair->zero_stock, to_detail);
      inset_balancelog(from, itr_pair->stock.code(), BALANCE_MOD_DEAL_ASK, itr_deal->amount, itr_pair->zero_stock, itr_deal->amount, itr_pair->zero_stock, to_detail);
   }
   else
   {
      //广告主是卖币方，广告主要付手续费，amount=100,fee=2 广告主要放币102,其中2币是手续费

      settle_stock(from, to, itr_deal->amount + fee, fee, "放币啦，卖币方把币打给买币的人，同时卖币方支付一笔手续费", is_bid_fee);
      auto itr_pair = get_market(itr_deal->pair);

      std::string to_detail(from.to_string());
      to_detail.append("(广告主)放币给").append(to.to_string()).append("(taker),");
      to_detail.append("deal_id :").append(std::to_string(itr_deal->id)).append(",ad_id:").append(std::to_string(itr_deal->maker_order_id)).append(",");
      to_detail.append("amount:").append(itr_deal->amount.to_string()).append(",");
      to_detail.append("price:").append(itr_deal->price.to_string()).append(",");
      to_detail.append("fee:").append(fee.to_string()).append(",");
      to_detail.append("广告主实际支付（amount+fee）:").append((itr_deal->amount + fee).to_string()).append(",");

      inset_balancelog(from, itr_pair->stock.code(), BALANCE_MOD_DEAL_BID, itr_deal->amount, fee, itr_deal->amount + fee, itr_pair->zero_stock, to_detail);
      inset_balancelog(to, itr_pair->stock.code(), BALANCE_MOD_DEAL_BID, itr_deal->amount, itr_pair->zero_stock, itr_deal->amount, itr_pair->zero_stock, to_detail);
   }

   //发一个转账action

   print("放币了,把币转给用户ok");

   update_dh_status(itr_deal->user, itr_deal->taker_id, status);
   update_dh_status(itr_deal->maker_user, itr_deal->maker_id, status);

   //修改deal的状态
   dealtable_.modify(itr_deal, _self, [&](deal &d) {
      d.status = status;
      d.status_str = get_deal_status_str(status);
      d.utime = current_time_point();
      d.source.append("|").append(reason).append(d.status_str);
   });

   auto itr_pair = get_market(itr_deal->pair);

   //修改广告的状态
   uint8_t new_ad_status = 0;
   adtable.modify(itr_ad, _self, [&](adorder &ad) {
      ad.utime = current_time_point();
      ad.source.append(reason);
      ad.freeze = ad.freeze - itr_deal->amount;
      ad.update_ad_status(itr_pair);
      new_ad_status = ad.status;
      //update_ad_status(ad, itr_pair);
   });
   if (new_ad_status == AD_STATUS_FINISHED)
   {
      update_user_ad_unfill_num(itr_ad->user, -1);
   }
   user_deal_fill(itr_deal->user); //这个用户此单完成
}

bool otcexchange::arb_can_cmd(const symbol_code &pair, deal_iter_t itr_deal, const std::string &reason)
{
   //在仲裁阶段是否可以取消和放币
   auto str_pair = pair.to_string();
   str_pair = lower_str(str_pair);
   arborder_index_t arbs(_self, name{str_pair}.value); //订单表
   auto it_order = arbs.require_find(itr_deal->id, "仲裁order找不到");
   auto total = it_order->yes_num + it_order->no_num;
   bool can = total == 0;
   if (can)
   {
      //更改仲裁订单状态
      arbs.modify(it_order, _self, [&](arborder &a) {
         a.utime = CURRENT_SEC;
         a.status = APPEAL_STATUS_CANCEL;
         a.status_str = APPEAL_STATUS_CANCEL_STR;
         a.source.append(reason);
      });

      //具体每个人
      auto map = it_order->map_person_task;
      for (auto &p : map)
      {
         arbtask_index_t arbtasks(_self, p.first.value); //表名，按用户分表，不是按pair分表
         auto it_task = arbtasks.require_find(p.second, "该仲裁员仲裁task找不到");
         check(it_task->status == ARBTASK_STATUS_CREATED, "已经仲裁过了,不能重复仲裁");
         arbtasks.modify(it_task, _self, [&](arbtask &t) {
            t.utime = CURRENT_SEC;
            t.status = ARBTASK_STATUS_CANCELED;
            t.status_str = ARBTASK_STATUS_CANCELED_STR;
            t.source.append("|").append(reason);
         });
      }
   }
   return can;
}

std::string otcexchange::get_user_nickname(name user)
{
   static const name cname = name{"otcsystem"};
   static const name notexist = name{""};
   otcsystem::user_table t(cname, cname.value);
   auto it = t.find(user.value);
   std::string res("");
   if (it != t.end())
   {
      res = it->nickname;
   }
   //print(user, "用户昵称是", res);
   return res;
}

std::string otcexchange::get_user_pay_accout_content(name asker, uint64_t pmt_id)
{
   static const name cname = name{"otcsystem"};
   static const name notexist = name{""};
   otcsystem::payment_table t(cname, asker.value);
   auto it = t.find(pmt_id);
   std::string res("");
   if (it != t.end())
   {
      res = it->ciphertext;
   }

   return res;
}

std::pair<asset, asset> otcexchange::get_market_price_range(market_iter_t itr_pair, name who)
{
   asset min = ZERO_ASSET(itr_pair->money);
   asset max = ZERO_ASSET(itr_pair->money);

   uint32_t flow_min = DEFAULT_PRICE_LOW_RATE;
   uint32_t flow_max = DEFAULT_PRICE_UP_RATE;

   static const name cname = name{"otcsystem"};

   static const name notexist = name{""};
   otcsystem::user_table table_user(cname, cname.value);
   auto it_user = table_user.find(who.value);
   if (it_user != table_user.end() && it_user->admin != notexist)
   {
      otcsystem::team_table tt(cname, it_user->admin.value);
      auto itt = tt.find(itr_pair->stock.code().raw());
      if (itt != tt.end())
      {

         flow_min = itt->price_limit_lower;

         flow_max = itt->price_limit_upper;
      }
   }

   print("交易对基本价格:", itr_pair->price_max.to_string());
   print("价格浮动下线比例:", flow_min);
   print("价格浮动上线比例:", flow_max);

   return std::make_pair(cal_fee(itr_pair->price_max, flow_min), cal_fee(itr_pair->price_max, flow_max));
}
std::pair<int64_t, int64_t> otcexchange::get_user_fee_rate(name user, const symbol_code &pair, uint8_t role)
{
   //查otcsystem的表  zz get table otcsystem otcsystem users 万分之1保存为1.
   uint32_t ask_fee_rate = 0;
   uint32_t bid_fee_rate = 0;

   auto itr_pair = get_market(pair);
   static const name cname = name{"otcsystem"};
   static const name notexist = name{""};
   otcsystem::user_table t(cname, cname.value);
   auto it = t.find(user.value);
   bool isfromuser = false;
   if (it != t.end() && it->admin != notexist)
   {
      otcsystem::team_table tt(cname, it->admin.value);
      auto itt = tt.find(itr_pair->stock.code().raw());
      if (itt != tt.end())
      {
         isfromuser = true;
         if (role == MARKET_ROLE_MAKER)
         {
            ask_fee_rate = itt->maker_ask;
            bid_fee_rate = itt->maker_bid;
         }
         else
         {
            ask_fee_rate = itt->taker_ask;
            bid_fee_rate = itt->taker_bid;
         }
      }
   }
   if (!isfromuser)
   {
      if (role == MARKET_ROLE_MAKER)
      {
         ask_fee_rate = itr_pair->maker_ask_fee_rate;
         bid_fee_rate = itr_pair->maker_bid_fee_rate;
      }
      else
      {
         ask_fee_rate = itr_pair->taker_ask_fee_rate;
         ;
         bid_fee_rate = itr_pair->taker_bid_fee_rate;
      }
   }

   return std::make_pair(ask_fee_rate, bid_fee_rate);
}

uint8_t otcexchange::get_level(const symbol_code &stock, asset balance)
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

asset otcexchange::get_fee(name user, const symbol_code &pair, asset amount, uint8_t askorbid, uint8_t role)
{
   asset result;

   auto fee_rate_pair = get_user_fee_rate(user, pair, role);
   //auto pr = std::pow(10, amount.symbol.precision());
   static auto pr4 = std::pow(10, 4);
   auto itr_pair = get_market(pair);

   if (askorbid == MARKET_ORDER_SIDE_ASK)
   {
      result = asset(static_cast<int64_t>(amount.amount * (fee_rate_pair.first) / pr4 + 0.5), itr_pair->stock);
   }
   else
   {
      result = asset(static_cast<int64_t>(amount.amount * (fee_rate_pair.second) / pr4 + 0.5), itr_pair->stock);
   }
   return result;
}

asset otcexchange::cal_asset(asset amount, uint32_t ratio)
{
   static auto pr2 = std::pow(10, 2);
   return asset(static_cast<int64_t>(amount.amount * ratio / pr2 + 0.5), amount.symbol);
}

ACTION otcexchange::newmarket(const symbol &stock,
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
                              uint8_t cancel_ad_num,
                              uint8_t control_price_role)
{

   require_auth(get_self()); //必须要求是合约账号的权限

   //入参校验
   print(control_price_role);

   check((control_price_role >= 1 && control_price_role <= 2), "control_price_role invalid");

   check(stock.is_valid(), "invalid symbol name");
   check(money.is_valid(), "invalid symbol name");

   check(amount_max.symbol == stock, "amount的symbol跟coin的不一致");
   check(amount_max.symbol == amount_min.symbol, "2个amount的symbol不一致");

   check(price_max.symbol == money, "price的symbol跟fiat的不一致");
   check(price_max.symbol == price_min.symbol, "2个price的symbol不一致");

   auto zero_stock = ZERO_ASSET(amount_max.symbol);
   auto zero_money = ZERO_ASSET(price_max.symbol);

   check(amount_min > zero_stock, ERR_MSG_PARAM_LE_ZERO(50102, amount_min, ERR_MSG_PARAM_PAIR_AMOUNT_MIN_MUST_GT_ZERO));
   check(amount_max > zero_stock, ERR_MSG_PARAM_LE_ZERO(50103, amount_max, ERR_MSG_PARAM_PAIR_AMOUNT_MAX_MUST_GT_ZERO));

   check(amount_min <= amount_max, ERR_MSG_PARAM_LT(50104, amount_max, amount_min, ERR_MSG_PARAM_PAIR_AMOUNT_MAX_MUST_GE_MIN));

   check(price_min > zero_money, ERR_MSG_PARAM_LE_ZERO(50105, price_min, ERR_MSG_PARAM_PAIR_PRICE_MIN_MUST_GT_ZERO));
   check(price_max > zero_money, ERR_MSG_PARAM_LE_ZERO(50106, price_max, ERR_MSG_PARAM_PAIR_PRICE_MAX_MUST_GT_ZERO));
   check(price_min <= price_max, ERR_MSG_PARAM_LT(50107, amount_max, amount_min, ERR_MSG_PARAM_PAIR_PRICE_MAX_MUST_GE_MIN));

   check(pay_timeout > 0, ERR_MSG_PARAM_LT_ZERO(50108, pay_timeout, ERR_MSG_PARAM_PAIR_PAY_TIMEOUT_MUST_GT_ZERO));
   check(cancel_ad_num > 0, ERR_MSG_PARAM_LT_ZERO(50109, cancel_ad_num, ERR_MSG_PARAM_PAIR_AD_CANCEL_NUM_MUST_GT_ZERO));

   std::string str_pair = std::move(stock.code().to_string() + money.code().to_string());

   symbol_code code_pair = symbol_code(str_pair);

   auto it = markets_.find(code_pair.raw());

   check(it == markets_.end(), ERR_MSG_CHECK_FAILED(50110, str_pair + ERR_MSG_PAIR_HAS_EXISTED)); //exchange pair has exist

   auto state = overview_.get();
   auto s1 = stock.code().to_string();
   auto m1 = money.code().to_string();
   symbol_code pair(s1 + m1);
   state.fiats.emplace(m1);
   state.stocks.emplace(s1);
   state.pairs.emplace(str_pair);
   state.precisions[s1] = stock.precision();
   state.precisions[m1] = money.precision();
   overview_.set(state, _self);

   markets_.emplace(_self, [&](market &m) {
      m.pair = code_pair;
      m.stock = stock;
      m.money = money;
      m.taker_ask_fee_rate = double_to_pow4(taker_ask_fee_rate);
      m.taker_bid_fee_rate = double_to_pow4(taker_bid_fee_rate);
      m.maker_ask_fee_rate = double_to_pow4(maker_ask_fee_rate);
      m.maker_bid_fee_rate = double_to_pow4(maker_bid_fee_rate);
      m.amount_min = amount_min;
      m.amount_max = amount_max;
      m.price_min = price_min;
      m.price_max = price_max;
      m.zero_stock = zero_stock;
      m.zero_money = zero_money;
      m.zero_rate = zero_stock;
      m.cancel_ad_num = cancel_ad_num;
      m.pay_timeout = pay_timeout;
      m.self_playcoin_time = self_playcoin_time;
      m.def_playcoin_time = def_playcoin_timeout;
      m.def_cancel_time = def_cancel_timeout;
      m.status = MARKET_STATUS_ON;
      m.str_status = MARKET_STATUS_ON_STR;
      m.nickname = str_pair;
      m.stockname = stock.code().to_string();
      m.moneyname = money.code().to_string();
      m.ctime = time_point_sec(current_time_point().sec_since_epoch());
      m.utime = time_point_sec(current_time_point().sec_since_epoch());
      m.base_price_role = 1;
      m.control_price_role = control_price_role;
   });

   //要根据当前价格算出其他交易对的价格的
   update_stock_other_base_price_role(code_pair);
}

ACTION otcexchange::modpairprice(name moder, const symbol_code &pair, asset new_price)
{
   require_auth(moder);
   auto it = markets_.find(pair.raw());
   check(it != markets_.end(), ERR_MSG_CHECK_FAILED(50110, pair.to_string().append(ERR_MSG_PAIR_NOT_EXIST)).c_str());
   check(new_price > it->zero_money, "price must >0 ");
   if (it->status != MARKET_STATUS_OFF)
   {
      markets_.modify(it, _self, [&](market &m) {
         m.price_max = new_price;
         m.price_min = new_price;
         m.utime = time_point_sec(current_time_point().sec_since_epoch());
         if (it->control_price_role == 1) //发币方修改价格
         {
            m.base_price_role = 1; //荣生为基准价
         }
      });
      if (it->control_price_role == 1) //更新其余交易对不再是基准价
      {
         update_stock_other_base_price_role(pair); //其他交易对改为是这个基准价
      }
   }
}

ACTION otcexchange::closemarket(const symbol_code &pair)
{
   require_auth(_self); //必须要求是合约账号的权限
   auto it = markets_.find(pair.raw());
   check(it != markets_.end(), ERR_MSG_CHECK_FAILED(50110, pair.to_string().append(ERR_MSG_PAIR_NOT_EXIST)).c_str());
   if (it->status != MARKET_STATUS_OFF)
   {
      markets_.modify(it, _self, [](market &m) {
         m.status = MARKET_STATUS_OFF;
         m.str_status = MARKET_STATUS_OFF_STR;
         m.utime = time_point_sec(current_time_point().sec_since_epoch());
      });
   }
}

ACTION otcexchange::openmarket(const symbol_code &pair)
{
   require_auth(_self); //必须要求是合约账号的权限
   auto it = markets_.find(pair.raw());
   check(it != markets_.end(), ERR_MSG_CHECK_FAILED(50110, pair.to_string().append(ERR_MSG_PAIR_NOT_EXIST)).c_str());
   if (it->status != MARKET_STATUS_ON)
   {
      markets_.modify(it, _self, [](market &m) {
         m.status = MARKET_STATUS_ON;
         m.str_status = MARKET_STATUS_ON_STR;
         m.utime = time_point_sec(current_time_point().sec_since_epoch());
      });
   }
}

ACTION otcexchange::getemailcode(name who, const std::string &email)
{
   require_auth(who);
   check(!email.empty(), "email地址为空");
   insert_emailcode(who, email);
}

ACTION otcexchange::regarbiter(name arbitername, const symbol_code &stock, asset mortgage, uint32_t online_begin, uint32_t online_end, const std::string &email, const std::string &verify_code)
{
   // 注册成为仲裁员
   require_auth(arbitername);
   check(!email.empty(), "email地址为空");
   check(isnotjudger(arbitername, stock), "该用户已经是终审者,不能在成为仲裁者");

   auto state = overview_.get();
   auto it_precision = state.precisions.find(stock.to_string());
   check(it_precision != state.precisions.end(), "代币不存在，请先创建代币的交易对");
   asset zero = asset(0, symbol(stock, it_precision->second));
   check(mortgage > zero, "抵押代币必须大于0");
   check(online_begin < online_end, "online_begin must <= online_end");
   check(online_begin >= 0 && online_begin <= 24 * 60 * 60, "online_begin must at[0h,24h]");
   check(online_end >= 0 && online_end <= 24 * 60 * 60, "online_end must at[0h,24h]");

   check(check_emailcode(arbitername, email, verify_code), "邮箱验证码不正确");

   auto it = arbsts_.require_find(stock.raw(), "找不到仲裁等级配置项");

   std::string str_stock = stock.to_string();
   str_stock = lower_str(str_stock);
   arbiter_index_t arbiters_(_self, name{str_stock}.value);

   auto it_user = arbiters_.find(arbitername.value);

   if (it_user == arbiters_.end())
   {
      auto level = get_level(stock, mortgage);
      check(level >= 1 && level <= 10, "level at [1,10]");
      arbiters_.emplace(_self, [&](arbiter &a) {
         a.account = arbitername;
         a.stock = stock;
         a.nickname = get_user_nickname(arbitername);
         a.balance = mortgage; //质押还剩下的代币余额
         a.mortgage = mortgage;
         a.total_revenue = zero;
         a.today_revenue = zero;
         a.today_predict_revenue = zero;
         a.today_revenue = zero;
         a.ctime = CURRENT_SEC;
         a.utime = a.ctime;
         a.last_arb_time = time_point_sec();
         a.total_num = 0; //仲裁总场次.默认是1 ，因为其作为了索引
         a.win_num = 0;   //仲裁胜利场次
         a.today_win_num = 0;
         a.is_work = 0;                    //是否正在仲裁中
         a.online_beg_time = online_begin; //在线开始时间[0,24] 精确到秒
         a.online_end_time = online_end;   //在线结束时间[0,24] 精确到秒
         a.level = level;                  //仲裁员配置信息
         a.status = ARBUSER_STATUS_REGED;  //1.注册中 2.注销 3
         a.email = email;
      });
   }
   else
   {
      auto level = get_level(stock, mortgage + it_user->balance);
      check(level >= 1 && level <= 10, "level at [1,10]");
      arbiters_.modify(it_user, _self, [&](arbiter &a) {
         a.balance += mortgage;
         a.mortgage += mortgage;
         a.utime = CURRENT_SEC;
         a.level = level;
         a.status = ARBUSER_STATUS_REGED;  //注册中
         a.online_beg_time = online_begin; //在线开始时间[0,24] 精确到秒
         a.online_end_time = online_end;   //在线结束时间[0,24] 精确到秒
         a.email = email;
      });
   }

   auto memo = std::string("成为仲裁者，抵押:").append(mortgage.to_string());
   print(memo);
   freeze_stock(arbitername, name{TOKEN_TEMP_ACCOUNT}, mortgage, USER_TYPE_ARB, memo);

   auto current_role = get_userrole(arbitername, stock);
   if (current_role == USER_ROLE_UNKOWN || current_role == USER_ROLE_ARB_UNREG)
   {
      insert_modify_userrole(arbitername, stock, USER_ROLE_ARB);
   }
   else if (current_role == USER_ROLE_EXCHANGE || current_role == USER_ROLE_EX_ARB_UNREG)
   {
      insert_modify_userrole(arbitername, stock, USER_ROLE_EX_ARB);
   }
}

ACTION otcexchange::modarbtime(name arbitername, const symbol_code &stock, uint32_t online_begin, uint32_t online_end)
{
   require_auth(arbitername);
   std::string str_stock = stock.to_string();
   str_stock = lower_str(str_stock);
   arbiter_index_t arbiters_(_self, name{str_stock}.value);

   auto it_user = arbiters_.find(arbitername.value);

   check(it_user != arbiters_.end(), "该用户该币种还没有注册成为仲裁者");

   check(it_user->status == ARBUSER_STATUS_REGED, "该用户该币种不是注册状态");

   arbiters_.modify(it_user, _self, [&](arbiter &a) {
      a.utime = CURRENT_SEC;
      a.online_beg_time = online_begin;
      a.online_end_time = online_end;
   });
}
ACTION otcexchange::modarbemail(name arbitername, const symbol_code &stock, const std::string &email, const std::string &verify_code)
{
   require_auth(arbitername);
   check(!email.empty(), "email地址为空");
   check(check_emailcode(arbitername, email, verify_code), "邮箱验证码不正确");

   std::string str_stock = stock.to_string();
   str_stock = lower_str(str_stock);
   arbiter_index_t arbiters_(_self, name{str_stock}.value);

   auto it_user = arbiters_.find(arbitername.value);

   check(it_user != arbiters_.end(), "该用户该币种还没有注册成为仲裁者");

   check(it_user->status == ARBUSER_STATUS_REGED, "该用户该币种不是注册状态");

   arbiters_.modify(it_user, _self, [&](arbiter &a) {
      a.utime = CURRENT_SEC;
      a.email = email;
   });
}

ACTION otcexchange::unregarbiter(name arbitername, const symbol_code &stock, const std::string &reason)
{

   require_auth(arbitername);
   std::string str_stock = stock.to_string();
   str_stock = lower_str(str_stock);
   arbiter_index_t arbiters_(_self, name{str_stock}.value);

   auto it_user = arbiters_.find(arbitername.value);

   check(it_user != arbiters_.end(), "该用户该币种还没有注册成为仲裁者");
   check(it_user->is_work == 0, "该用户该币种还有仲裁任务");
   check(it_user->status == ARBUSER_STATUS_REGED, "该用户该币种不是注册状态");
   check((CURRENT_SEC.sec_since_epoch() - it_user->last_arb_time.sec_since_epoch()) > DAY_SECONDS, "必须在最近一次仲裁24h后才能注销仲裁员");

   unfreeze_stock(name{TOKEN_TEMP_ACCOUNT}, arbitername, it_user->balance, USER_TYPE_ARB, "仲裁员注销");

   arbiters_.modify(it_user, _self, [&](arbiter &a) {
      a.utime = CURRENT_SEC;
      a.status = ARBUSER_STATUS_UNREGED;
      a.balance = a.balance - a.balance;
      a.level = 0;
   });

   auto current_role = get_userrole(arbitername, stock);
   if (current_role == USER_ROLE_ARB)
   {
      insert_modify_userrole(arbitername, stock, USER_ROLE_ARB_UNREG);
   }
   else if (current_role == USER_ROLE_EX_ARB)
   {
      insert_modify_userrole(arbitername, stock, USER_ROLE_EX_ARB_UNREG);
   }
}

ACTION otcexchange::regjuder(name judgername, const symbol_code &stock, uint32_t online_begin, uint32_t online_end, const std::string &email, const std::string &verify_code)
{
   // 注册成为仲裁员
   require_auth(judgername);
   check(isnotarber(judgername, stock), "该用户已经是终审者,不能在成为仲裁者");

   auto state = overview_.get();
   auto it_precision = state.precisions.find(stock.to_string());
   check(it_precision != state.precisions.end(), "代币不存在，请先创建代币的交易对");

   check(online_begin < online_end, "online_begin must <= online_end");
   check(online_begin >= 0 && online_begin <= 24 * 60 * 60, "online_begin must at[0h,24h]");
   check(online_end >= 0 && online_end <= 24 * 60 * 60, "online_end must at[0h,24h]");
   asset zero = asset(0, symbol(stock, it_precision->second));

   std::string str_stock = stock.to_string();
   str_stock = lower_str(str_stock);
   judger_index_t t_(_self, name{str_stock}.value);

   auto it_user = t_.find(judgername.value);

   if (it_user == t_.end())
   {
      t_.emplace(_self, [&](judger &a) {
         a.account = judgername;
         a.nickname = get_user_nickname(judgername);
         a.stock = stock;
         a.balance = zero; //质押还剩下的代币余额
         a.mortgage = zero;

         a.ctime = CURRENT_SEC;
         a.utime = a.ctime;
         a.last_judge_time = time_point_sec();
         a.total_num = 0;
         a.cancelplay_num = 0;
         a.playcoin_num = 0;

         a.is_work = 0;                    //是否正在仲裁中
         a.online_beg_time = online_begin; //在线开始时间[0,24] 精确到秒
         a.online_end_time = online_end;   //在线结束时间[0,24] 精确到秒
         a.level = 0;

         a.status = JUDUSER_STATUS_REGED; //1.注册中 2.注销 3
         a.email = email;
      });
   }
   else
   {
      t_.modify(it_user, _self, [&](judger &a) {
         a.utime = CURRENT_SEC;
         a.status = JUDUSER_STATUS_REGED;
      });
   }

   auto current_role = get_userrole(judgername, stock);
   if (current_role == USER_ROLE_UNKOWN || current_role == USER_ROLE_JUDGE_UNREG)
   {

      insert_modify_userrole(judgername, stock, USER_ROLE_JUDGE);
   }
   else if (current_role == USER_ROLE_EXCHANGE || current_role == USER_ROLE_EX_JUDGE_UNREG)
   {

      insert_modify_userrole(judgername, stock, USER_ROLE_EX_JUDGE);
   }
}
ACTION otcexchange::unregjudger(name judgername, const symbol_code &stock, const std::string &reason)
{
   require_auth(judgername);
   std::string str_stock = stock.to_string();
   str_stock = lower_str(str_stock);
   judger_index_t t_(_self, name{str_stock}.value);

   auto it_user = t_.find(judgername.value);
   check(it_user != t_.end(), "该用户该币种还没有注册成为仲裁者");
   check(it_user->is_work == 0, "该用户该币种还有仲裁任务");
   check(it_user->status == JUDUSER_STATUS_REGED, "该用户该币种不是注册状态");

   //软删除

   t_.modify(it_user, _self, [&](judger &a) {
      a.utime = CURRENT_SEC;
      a.status = JUDUSER_STATUS_UNREGED;
      a.balance = a.balance - a.balance;
      a.level = 0;
   });

   auto current_role = get_userrole(judgername, stock);
   if (current_role == USER_ROLE_JUDGE)
   {
      insert_modify_userrole(judgername, stock, USER_ROLE_JUDGE_UNREG);
   }
   else if (current_role == USER_ROLE_EX_JUDGE)
   {
      insert_modify_userrole(judgername, stock, USER_ROLE_EX_JUDGE_UNREG);
   }
}

ACTION otcexchange::newarbst(const symbol_code &stock, const std::vector<asset> &vec_level)
{
   require_auth(_self);
   check(!vec_level.empty() && vec_level.size() <= 10, "最多10档");
   auto it = arbsts_.find(stock.raw()); //primary_key

   auto state = overview_.get();
   auto it1 = state.precisions.find(stock.to_string());
   check(it1 != state.precisions.end(), "代币不存在，请先创建代币的交易对");

   asset zero = asset(0, symbol(stock, it1->second));
   for (auto &item : vec_level)
   {
      check(item != zero, "vec_level 存在非法的代币");
   }
   auto vec = vec_level;
   //对vector排序
   std::sort(vec.begin(), vec.end());

   if (it == arbsts_.end())
   {
      arbsts_.emplace(_self, [&](arbst &s) {
         s.stock = stock;
         s.vec_level = vec;
         s.ctime = CURRENT_SEC;
         s.utime = CURRENT_SEC;
      });
   }
}

ACTION otcexchange::rmarbst(const symbol_code &stock)
{
   require_auth(_self);
   auto t = stock.to_string();
   t.append(" arbst not exist");
   auto it = arbsts_.require_find(stock.raw(), t.c_str());
   arbsts_.erase(it);
}

ACTION otcexchange::rmarbsts()
{
   require_auth(_self);
   erase_arbsts();
}

ACTION otcexchange::rmdhs(name who)
{
   require_auth(_self);
   deallog_index_t dh_(_self, who.value);

   auto it = dh_.begin();

   while (it != dh_.end())
   {
      it = dh_.erase(it);
   }
   print(who, "的deal history clear ok");
}

ACTION otcexchange::rmarbiters(const symbol_code &stock)
{
   require_auth(_self);
   std::string str_stock = stock.to_string();
   str_stock = lower_str(str_stock);
   arbiter_index_t d_(_self, name{str_stock}.value);

   auto it = d_.begin();

   while (it != d_.end())
   {
      it = d_.erase(it);
   }
   print(stock, "的仲裁员表 clear ok");
}

ACTION otcexchange::rmjudgers(const symbol_code &stock)
{
   require_auth(_self);
   std::string str_stock = stock.to_string();
   str_stock = lower_str(str_stock);
   judger_index_t d_(_self, name{str_stock}.value);

   auto it = d_.begin();

   while (it != d_.end())
   {
      it = d_.erase(it);
   }
   print(stock, "的终审员表 clear ok");
}

ACTION otcexchange::rmarbtasks(name arbitername)
{
   require_auth(_self);
   arbtask_index_t d_(_self, arbitername.value); //表名，按用户分表，不是按pair分表
   auto it = d_.begin();

   while (it != d_.end())
   {
      it = d_.erase(it);
   }
   print(arbitername, "的仲裁任务表 clear ok");
}

ACTION otcexchange::rmmarket(const symbol_code &stock, const symbol_code &money)
{
   require_auth(_self);
   auto s1 = stock.to_string();
   auto m1 = money.to_string();
   symbol_code pair(s1 + m1);
   auto it = get_market(pair);
   markets_.erase(it);

   auto state = overview_.get();
   state.fiats.erase(m1);
   state.stocks.erase(s1);
   state.pairs.erase(s1 + m1);
   state.precisions.erase(m1);
   state.precisions.erase(s1);
   overview_.set(state, _self);

   print("delete one market finish\n");
}

ACTION otcexchange::rmmarkets()
{
   require_auth(_self); //必须要求是合约账号的权限
   erase_markets();     //table是合约范围内的
   overview_.remove();

   print("delete all market finish\n");
}
ACTION otcexchange::rmad(const symbol_code &pair, const std::string &side, uint64_t ad_id)
{
   require_auth(get_self());
   auto adtable_ = get_adtable(pair, side);
   auto it = adtable_.require_find(ad_id, ERR_MSG_AD_NOT_EXIST);
   adtable_.erase(it);
}

ACTION otcexchange::rmads(const symbol_code &pair, const std::string &side)
{
   require_auth(get_self());
   erase_adorders(pair, side);
}

ACTION otcexchange::rmnotfills()
{
   require_auth(_self);
   {
      adnotfill_index_t t(_self, _self.value);
      auto it = t.begin();
      while (it != t.end())
      {
         it = t.erase(it);
      }
   }
   {
      dealnotfill_index_t t(_self, _self.value);
      auto it = t.begin();
      while (it != t.end())
      {
         it = t.erase(it);
      }
   }
}

ACTION otcexchange::rmdeal(const symbol_code &pair, uint64_t id)
{
   require_auth(get_self());
   auto str_pair = pair.to_string();
   str_pair = lower_str(str_pair);
   deal_index_t dealtable_(_self, name{str_pair}.value);
   auto it = dealtable_.require_find(id, "吃单找不到");
   dealtable_.erase(it);
}

ACTION otcexchange::rmdeals(const symbol_code &pair)
{
   require_auth(get_self());
   erase_deals(pair);
}

ACTION otcexchange::canmodpay(name user)
{
   dealnotfill_index_t deal(_self, _self.value);
   auto it_deal = deal.find(user.value);
   check(it_deal == deal.end(), user.to_string().append("有未完成的交易，不能修改支付方式"));

   adnotfill_index_t ad(_self, _self.value);
   auto it_ad = ad.find(user.value);
   check(it_ad == ad.end() || it_ad->num == 0, user.to_string().append("有未完成的广告，不允许修改收付款方式"));
}

ACTION otcexchange::moduserrole(name user, const symbol_code &stock, uint8_t role)
{
   require_auth(_self);
   insert_modify_userrole(user, stock, role);
}
ACTION otcexchange::erauserrole(name user, const symbol_code &stock, uint8_t role)
{
   require_auth(_self);
   delete_userrole(user, stock, role);
}

bool otcexchange::isnotarber(name user, const symbol_code &stock)
{

   bool res = true;
   userrole_index_t t(_self, user.value);

   auto it = t.find(stock.raw());
   if (it != t.end())
   {
      if (it->role == USER_ROLE_ARB || it->role == USER_ROLE_EX_ARB)
      {
         res = false;
      }
   }
   return res;
}

bool otcexchange::isnotjudger(name user, const symbol_code &stock)
{
   bool res = true;
   userrole_index_t t(_self, user.value);

   auto it = t.find(stock.raw());
   if (it != t.end())
   {
      if (it->role == USER_ROLE_JUDGE || it->role == USER_ROLE_EX_JUDGE)
      {
         res = false;
      }
   }
   return res;
}

uint8_t otcexchange::get_userrole(name user, const symbol_code &stock)
{
   userrole_index_t t(_self, user.value);
   auto it = t.find(stock.raw());
   return it != t.end() ? it->role : 0;
}

void otcexchange::delete_userrole(name user, const symbol_code &stock, uint8_t role)
{
   check(role >= USER_ROLE_EXCHANGE && role <= USER_ROLE_EX_JUDGE_BALACKLIST, "role value invalid");
   userrole_index_t t(_self, user.value);
   auto it = t.require_find(stock.raw(), "该币种下暂无该用户角色");
   t.erase(it);
}

void otcexchange::insert_modify_userrole(name user, const symbol_code &stock, uint8_t role)
{
   check(role >= USER_ROLE_EXCHANGE && role <= USER_ROLE_EX_JUDGE_BALACKLIST, "role value invalid");
   userrole_index_t t(_self, user.value);
   auto it = t.find(stock.raw());
   if (it == t.end())
   {
      t.emplace(_self, [&](userrole &r) {
         r.stock = stock;
         r.account = user;
         r.role = role;
         r.ctime = CURRENT_SEC;
         r.utime = r.ctime;
      });
   }
   else
   {

      t.modify(it, _self, [&](userrole &r) {
         r.utime = CURRENT_SEC;
         r.role = role;
      });
   }
}

void otcexchange::inset_balancelog(name user, const symbol_code &stock, const std::string &business, asset amount, asset fee, asset change, asset balance, const std::string &detail)
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

void otcexchange::erase_balancelogs(name user)
{
   balancelog_index_t t(_self, user.value); //得到表
   auto it = t.begin();
   while (it != t.end())
   {
      it = t.erase(it);
   }
}

void otcexchange::insert_emailcode(name who, const std::string &email)
{
   emailcode_index_t t(_self, who.value);
   t.emplace(_self, [&](emailcode &ec) {
      ec.id = t.available_primary_key();
      ec.email = email;
      ec.ctime = CURRENT_SEC;
      auto gen = random_gen(who);
      ec.code = std::to_string(gen.range(1000, 9999));
   });
}
bool otcexchange::check_emailcode(name who, const std::string &email, const std::string &code)
{
   emailcode_index_t t(_self, who.value);
   auto it = t.rbegin();
   bool res = false;
   if (it != t.rend())
   {
      if (email == it->email && code == it->code)
      {
         if (CURRENT_SEC.sec_since_epoch() - it->ctime.sec_since_epoch() <= 60 * 10)
         {
            res = true;
         }
      }
   }
   return true;
}

void otcexchange::put_arbpool_fee(const symbol &stock, time_point_sec now, const std::set<name> &win_arbiters, asset fee, deal_iter_t itr_deal)
{
   std::string str_stock = stock.code().to_string();
   str_stock = lower_str(str_stock);
   arbpool_index_t t(_self, name{str_stock}.value);
   auto zero_asset = ZERO_ASSET(stock);
   auto day = day_start(now);
   auto it = t.find(day.sec_since_epoch());
   if (it != t.end())
   {
      t.modify(it, _self, [&](arbpool &a) {
         a.utime = CURRENT_SEC;
         a.total_arb_fee += fee;

         for (auto &item : win_arbiters)
         {
            if (a.personal_arb_fee.find(item) == a.personal_arb_fee.end())
            {
               a.personal_arb_fee.emplace(item, zero_asset);
            }
         }
      });
   }
   else
   {
      t.emplace(_self, [&](arbpool &a) {
         a.date = day;
         a.stock = stock.code();
         a.ctime = CURRENT_SEC;
         a.utime = a.ctime;
         a.total_arb_fee = fee;
         for (auto &item : win_arbiters)
         {
            if (a.personal_arb_fee.find(item) == a.personal_arb_fee.end())
            {
               a.personal_arb_fee.emplace(item, zero_asset);
            }
         }
      });
   }
}

std::string otcexchange::get_deal_status_str(uint8_t status)
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

ACTION otcexchange::rmappeals(const symbol_code &pair)
{
   require_auth(_self);
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

ACTION otcexchange::rmarborders(const symbol_code &pair)
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

ACTION otcexchange::rmjudges(const symbol_code &pair)
{
   require_auth(_self);
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
ACTION otcexchange::rmjudorders(const symbol_code &pair)
{
   require_auth(_self);
   auto str_pair = pair.to_string();
   str_pair = lower_str(str_pair);
   judgeorder_index_t t(_self, name{str_pair}.value);
   auto it = t.begin();
   while (it != t.end())
   {
      it = t.erase(it);
   }
   print("judges 表已经清理完成");
}
ACTION otcexchange::rmjudtasks(name judgername)
{

   require_auth(_self);
   judgetask_index_t d_(_self, judgername.value); //表名，按用户分表，不是按pair分表
   auto it = d_.begin();

   while (it != d_.end())
   {
      it = d_.erase(it);
   }
   print(judgername, "的仲裁任务表 clear ok");
}
//自己放币：已付款待支付中->放币中->放币完成
//仲裁放币：仲裁中->仲裁放币中->放币完成
//终审放币：仲裁取消或放币->终审放币中->放币完成

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