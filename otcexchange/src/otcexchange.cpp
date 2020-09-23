#include "otcexchange.hpp"
#include <eosio/print.hpp>
#include <algorithm>

//#include "nlohmann/json.hpp"
//using namespace nlohmann;

ACTION otcexchange::hi(name nm)
{
   print_f("Name : %\n", nm);
}

ACTION otcexchange::newmarket(const symbol &stock,
                              const symbol &money,
                              asset taker_fee_rate,
                              asset maker_fee_rate,
                              asset amount_min,
                              asset amount_max,
                              asset price_min,
                              asset price_max,
                              uint32_t pay_timeout,
                              uint8_t cancel_ad_num)
{

   require_auth(get_self()); //必须要求是合约账号的权限

   //入参校验

   check(stock.is_valid(), "invalid symbol name");
   check(money.is_valid(), "invalid symbol name");

   auto zero_stock = ZERO_ASSET(stock);
   auto zero_money = ZERO_ASSET(money);

   check(taker_fee_rate >= zero_stock, ERR_MSG_PARAM_LT_ZERO(50100, taker_fee_rate, ERR_MSG_PARAM_PAIR_TAKER_FEE_RATE_MUST_GE_ZERO));
   check(maker_fee_rate >= zero_stock, ERR_MSG_PARAM_LT_ZERO(50101, maker_fee_rate, ERR_MSG_PARAM_PAIR_MAKER_FEE_RATE_MUST_GE_ZERO));

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

   auto it = markettable_.find(code_pair.raw());

   check(it == markettable_.end(), ERR_MSG_CHECK_FAILED(50110, str_pair + ERR_MSG_PAIR_HAS_EXISTED)); //exchang pair has exist

   markettable_.emplace(_self, [&](market &m) {
      m.pair = code_pair;
      m.stock = stock;
      m.money = money;
      m.taker_fee_rate = taker_fee_rate;
      m.maker_fee_rate = maker_fee_rate;
      m.amount_min = amount_min;
      m.amount_max = amount_max;
      m.price_min = price_min;
      m.price_max = price_max;
      m.zero_stock = zero_stock;
      m.zero_money = zero_money;
      m.cancel_ad_num = cancel_ad_num;
      m.pay_timeout = pay_timeout;
      m.status = MARKET_STATUS_ON;
      m.str_status = MARKET_STATUS_ON_STR;
      m.ctime = time_point_sec(current_time_point().sec_since_epoch());
      m.utime = time_point_sec(current_time_point().sec_since_epoch());
   });
}

ACTION otcexchange::closemarket(const symbol_code &pair)
{
   require_auth(_self); //必须要求是合约账号的权限
   auto it = markettable_.find(pair.raw());
   check(it != markettable_.end(), ERR_MSG_CHECK_FAILED(50110, pair.to_string().append(ERR_MSG_PAIR_NOT_EXIST)).c_str());
   if (it->status != MARKET_STATUS_OFF)
   {
      markettable_.modify(it, _self, [](market &m) {
         m.status = MARKET_STATUS_OFF;
         m.str_status = MARKET_STATUS_OFF_STR;
         m.utime = time_point_sec(current_time_point().sec_since_epoch());
      });
   }
}

ACTION otcexchange::openmarket(const symbol_code &pair)
{
   require_auth(_self); //必须要求是合约账号的权限
   auto it = markettable_.find(pair.raw());
   check(it != markettable_.end(), ERR_MSG_CHECK_FAILED(50110, pair.to_string().append(ERR_MSG_PAIR_NOT_EXIST)).c_str());
   if (it->status != MARKET_STATUS_ON)
   {
      markettable_.modify(it, _self, [](market &m) {
         m.status = MARKET_STATUS_ON;
         m.str_status = MARKET_STATUS_ON_STR;
         m.utime = time_point_sec(current_time_point().sec_since_epoch());
      });
   }
}

ACTION otcexchange::putadorder(const symbol_code &pair,
                               const std::string &side,
                               name user,
                               asset price,
                               asset amount,
                               asset amount_min,
                               asset amount_max,
                               const std::string &source)
{
   require_auth(user);

   auto itr_pair = get_open_market(pair);

   check(price > itr_pair->zero_money, ERR_MSG_PARAM_LT_ZERO(50210, price, ERR_MSG_PARAM_AD_PRICE_MUST_GT_ZERO));
   check(amount > itr_pair->zero_stock, ERR_MSG_PARAM_LT_ZERO(50211, amount, ERR_MSG_PARAM_AD_AMOUNT_MUST_GT_ZERO));

   check(amount_min > itr_pair->zero_stock, ERR_MSG_PARAM_LE_ZERO(50212, amount_min, ERR_MSG_PARAM_AD_AMOUNT_MIN_MUST_GT_ZERO));
   check(amount_max > itr_pair->zero_stock, ERR_MSG_PARAM_LE_ZERO(50213, amount_max, ERR_MSG_PARAM_AD_AMOUNT_MAX_MUST_GT_ZERO));
   check(amount_min <= amount_max, ERR_MSG_PARAM_LE(50215, amount_min, amount_max, ERR_MSG_PARAM_AD_AMOUNT_MAX_MUST_GE_MIN));
   check(amount_max <= amount, ERR_MSG_PARAM_LE(50216, amount_max, amount, ERR_MSG_PARAM_AD_AMOUNT_MUST_GE_AMOUNT_MAX));

   check(amount >= itr_pair->amount_min, "广告交易数量小于配置最小交易数量");
   check(amount <= itr_pair->amount_max, "广告交易数量大于配置最大交易数量");

   //交易对的属性比对
   check(price >= itr_pair->price_min, "广告交易价格小于合约配置最低价");
   check(price <= itr_pair->price_max, "广告交易价格大于合约配置最高价");

   check((side == MARKET_ORDER_SIDE_ASK_STR || side == MARKET_ORDER_SIDE_BID_STR), ERR_MSG_SIDE);

   auto side_int = side_to_uint(side);
   //是卖，判断余额够不够
   if (side_int == MARKET_ORDER_SIDE_ASK)
   {
      print("是卖，可用余额不足");
   }

   //要注意name类型的长度限制
   std::string scope = pair.to_string() + MARKET_ROLE_MAKER_STR + side;
   transform(scope.begin(), scope.end(), scope.begin(), ::tolower); //转成小写

   order_index_t adtable_(_self, name{scope}.value);

   adtable_.emplace(user, [&](adorder &order) {
      order.id = adtable_.available_primary_key();

      order.user = user;                               //订单所属用户，注意是name类型
      order.pair = pair;                               //
      order.ctime = current_time_point();              //订单创建时间，精确到微秒
      order.utime = order.ctime;                       //订单更新时间，精确到微秒
      order.status = AD_STATUS_ONTHESHELF;             //订单状态，上架中
      order.side = side_int;                           //买卖类型，1卖 2买
      order.type = MARKET_ORDER_TYPE_LIMIT;            //订单类型，1限价 2市价
      order.role = MARKET_ROLE_MAKER;                  //订单类型，1挂单 2吃单
      order.price = price;                             //订单交易价格
      order.amount = amount;                           //订单交易数量
      order.amount_min = amount_min;                   //订单最小成交数量,默认是2
      order.amount_max = amount_max;                   //订单最小成交数量,默认是2
      order.taker_fee_rate = itr_pair->taker_fee_rate; //吃单的手续费率
      order.maker_fee_rate = itr_pair->maker_fee_rate; //挂单的手续费率
      order.left = amount;                             //剩余多少数量未成交
      order.freeze = itr_pair->zero_stock;             //冻结的stock或者money
      order.deal_fee = itr_pair->zero_stock;           //累计的交易手续费
      order.deal_stock = itr_pair->zero_stock;         //累计的交易sotck数量
      order.deal_money = itr_pair->zero_money;         //累计的交易money
      order.source = source;                           //备注信息，订单来源
   });
}

ACTION otcexchange::offselfad(const symbol_code &pair,
                              const std::string &side,
                              uint64_t ad_id,
                              const std::string &reason)
{
   //如果是卖币的广告单，不能取消

   auto adtable_ = get_adtable(pair, side);
   auto it = adtable_.require_find(ad_id, ORDER_NOT_EXIST_STR);
   auto user = it->user;
   require_auth(user); //鉴权
   //哪种条件才能取下架合约
   //检查当前的广告下面的吃单最新状态，看是否都完成了或者都取消了
   deal_index_t dealtable_(_self, name{pair.to_string()}.value);

   for (auto &id : it->vec_deal)
   {
      auto d = dealtable_.require_find(id, "吃单找不到");
      check((d->status == DEAL_STATUS_UNPAID_MAN_CANCELED ||
             d->status == DEAL_STATUS_UNPAID_TIMEOUT_CANCELED ||
             d->status == DEAL_STATUS_PAID_PLAYCOIN_ED),
            "");
   }

   adtable_.modify(it, user, [&](adorder &ad) {
      ad.status = AD_STATUS_MAN_OFFTHESHELF; //手动下架
      ad.utime = current_time_point();
      ad.source.append("手动下架").append(reason);
      //把之前用户冻结的钱解冻
   });
}

ACTION otcexchange::puttkorder(const symbol_code &pair,
                               const std::string &side,
                               name user,
                               asset price,
                               asset amount,
                               uint64_t ad_id,
                               const std::string &source)

{

   require_auth(user);

   auto itr_pair = markettable_.find(pair.raw());
   check(itr_pair != markettable_.end(), PAIR_NOT_EXIST_STR);

   check(price.amount > 0, ERR_CHECK_PRICE_GREAT_ZERO);
   check(amount.amount > 0, ERR_CHECK_AMOUNT_GREAT_ZERO);

   check(price.symbol == itr_pair->money, "");  //法币
   check(amount.symbol == itr_pair->stock, ""); //代币

   //交易对的属性比对

   //校验side是否合法
   check((side == MARKET_ORDER_SIDE_ASK_STR || side == MARKET_ORDER_SIDE_BID_STR), SIDE_INVALID_STR);
   auto side_int = side_to_uint(side);
   //1.找到对手方订单的买卖的方向
   std::string mk_side("");
   if (side == MARKET_ORDER_SIDE_ASK_STR)
   {
      mk_side = MARKET_ORDER_SIDE_BID_STR;
   }
   else
   {
      mk_side = MARKET_ORDER_SIDE_ASK_STR;
   }

   std::string scope_maker = pair.to_string() + MARKET_ROLE_MAKER_STR + mk_side;
   transform(scope_maker.begin(), scope_maker.end(), scope_maker.begin(), ::tolower); //scope要求是小写

   order_index_t adtable_(_self, name{scope_maker}.value);
   //要求挂单必须存在
   auto mk_it = adtable_.require_find(ad_id, MAKER_ORDER_NOR_EXIST_STR);

   //获取挂单的用户
   const auto &mk_user = mk_it->user;
   //必须不是自成交
   check(mk_user != user, ERR_FORBID_SELF_EXCHANGE);

   //判断数量
   check(amount >= mk_it->amount_min, "吃单数量小于广告单最小交易数量");
   check(amount <= mk_it->amount_max, "吃单交易数量大于广告单最大交易数量");

   auto tk_side = side_to_uint(side);

   //判断数量是不是合法

   if (tk_side == MARKET_ORDER_SIDE_BID)
   {
      check(price >= mk_it->price, "我出的买价必须>=卖价,才能撮合");
      auto surplus = mk_it->left - mk_it->freeze;
      check(amount <= surplus, ERR_NOT_ENOUGH_TOKEN_TO_SELL); //我是买币方,对手方为卖币,看卖币方够不够
   }
   else
   {
      print("吃单卖币，可用余额足不足呢？");
      //我是卖币方,看我是不是还有币可卖,如果有,则直接冻结,这里发送一个内联调用action,如果执行成功，就继续，失败，就不往下走
      check(price <= mk_it->price, "我的卖价必须<=买价,才能撮合");
   }

   //上面判断都通过，生成一个成交记录，也就是吃单详情,属于合约，scope是交易对

   std::string scope = pair.to_string();
   transform(scope.begin(), scope.end(), scope.begin(), ::tolower);

   deal_index_t dealtable_(_self, name{scope}.value);

   uint64_t deal_id = dealtable_.available_primary_key();

   dealtable_.emplace(user, [&](deal &deal) {
      deal.id = deal_id;
      deal.user = user;    //订单所属用户，注意是name类型
      deal.side = tk_side; //买卖类型，1卖 2买

      deal.type = MARKET_ORDER_TYPE_LIMIT; //订单类型，1限价 2市价
      deal.role = MARKET_ROLE_TAKER;       //订单类型，1挂单 2吃单
      deal.ctime = current_time_point();   //订单创建时间，精确到微秒
      deal.utime = deal.ctime;             //订单更新时间，精确到微秒

      deal.maker_user = mk_user;
      deal.maker_order_id = ad_id;

      deal.price = price;   //订单交易价格
      deal.amount = amount; //订单交易数量

      deal.quota = asset(0, itr_pair->money);
      deal.ask_fee = asset(0, itr_pair->stock);
      deal.bid_fee = asset(0, itr_pair->stock);
      deal.status = DEAL_STATUS_UNPAID; //待支付状态
      deal.pay_timeout = itr_pair->pay_timeout;
      deal.pair = pair;
      deal.source = source;
   });

   //修改广告挂单的状态
   adtable_.modify(mk_it, mk_it->user, [&itr_pair, &deal_id, &amount, &tk_side](adorder &o) {
      o.vec_deal.emplace_back(deal_id);
      o.source.append("|").append("被吃单");
      o.utime = current_time_point();

      if (tk_side == MARKET_ORDER_SIDE_BID)
      {
         //mk是卖币方
         o.freeze = o.freeze + amount; //o.freeze是在处于交易中的卖的币，正在等待对方法币支付
      }

      o.left = o.left - amount;

      otcexchange::update_ad_status(o, itr_pair);
   });
}

ACTION otcexchange::canceldeal(const symbol_code &pair, uint64_t deal_id, const std::string &reason)
{
   auto itr_pair = markettable_.find(pair.raw());
   check(itr_pair != markettable_.end(), PAIR_NOT_EXIST_STR);

   deal_index_t dealtable_(_self, name{pair.to_string()}.value);
   auto it = dealtable_.require_find(deal_id, "吃单找不到");
   auto user = it->user;
   require_auth(user);
   check(it->status == DEAL_STATUS_UNPAID, "只有处于待支付状态的订单才能取消");
   check(it->side == MARKET_ORDER_SIDE_BID, "只有是买币方才能取消");

   std::string mk_side("");
   if (it->side == MARKET_ORDER_SIDE_ASK)
   {
      mk_side = MARKET_ORDER_SIDE_BID_STR;
   }
   else
   {
      mk_side = MARKET_ORDER_SIDE_ASK_STR;
   }

   auto adtable_ = get_adtable(pair, mk_side);
   auto itr_ad = adtable_.require_find(it->maker_order_id, "对应的广告订单不存在");
   adtable_.modify(itr_ad, itr_ad->user, [&](adorder &ad) {
      if (itr_ad->side == MARKET_ORDER_SIDE_ASK) //如果广告方式卖币，加回币
      {
         ad.left = ad.left + it->amount;
         ad.freeze = ad.freeze - it->amount;
         update_ad_status(ad, itr_pair);
      }
   });

   //deal 表
   dealtable_.modify(it, user, [&](deal &d) {
      d.status = DEAL_STATUS_UNPAID_MAN_CANCELED;
      d.utime = current_time_point();
      d.source.append("手动取消交易 by ").append(reason);
   });
   //广告表
}

ACTION otcexchange::paydeal(const symbol_code &pair, uint64_t deal_id) //支付
{

   deal_index_t dealtable_(_self, name{pair.to_string()}.value);
   auto it = dealtable_.require_find(deal_id, "吃单找不到");
   auto user = it->user;
   require_auth(user);
   check(it->status == DEAL_STATUS_UNPAID, "订单必须处于待支付状态");
   dealtable_.modify(it, user, [&](deal &d) {
      d.status = DEAL_STATUS_PAID;
   });
}
ACTION otcexchange::playcoin(const symbol_code &pair, uint64_t deal_id) //放币操作
{
   deal_index_t dealtable_(_self, name{pair.to_string()}.value);
   auto it = dealtable_.require_find(deal_id, "吃单找不到");
   auto user = it->user;
   require_auth(user);
   check(it->status == DEAL_STATUS_PAID || it->status == DEAL_STATUS_PAID_ARBIARATE, "订单必须处于已支付状态");
   dealtable_.modify(it, user, [&](deal &d) {
      d.status = DEAL_STATUS_PAID_PLAYCOIN_ING; //放币中
   });
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