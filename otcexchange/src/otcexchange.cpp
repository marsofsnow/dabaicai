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
                              uint32_t self_playcoin_time,
                              uint32_t def_cancel_timeout,
                              uint32_t def_playcoin_timeout,
                              uint8_t cancel_ad_num)
{

   require_auth(get_self()); //必须要求是合约账号的权限

   //入参校验

   check(stock.is_valid(), "invalid symbol name");
   check(money.is_valid(), "invalid symbol name");
   check(taker_fee_rate.symbol.code() == stock.code(), "费率的symbol跟coin的不一致");
   check(taker_fee_rate.symbol == maker_fee_rate.symbol, "2个费率的symbol不一致");

   check(amount_max.symbol == stock, "amount的symbol跟coin的不一致");
   check(amount_max.symbol == amount_min.symbol, "2个amount的symbol不一致");

   check(price_max.symbol == money, "price的symbol跟fiat的不一致");
   check(price_max.symbol == price_min.symbol, "2个price的symbol不一致");

   auto zero_stock = ZERO_ASSET(amount_max.symbol);
   auto zero_money = ZERO_ASSET(price_max.symbol);
   auto zero_rate = ZERO_ASSET(taker_fee_rate.symbol);

   check(taker_fee_rate >= zero_rate, ERR_MSG_PARAM_LT_ZERO(50100, taker_fee_rate, ERR_MSG_PARAM_PAIR_TAKER_FEE_RATE_MUST_GE_ZERO));
   check(maker_fee_rate >= zero_rate, ERR_MSG_PARAM_LT_ZERO(50101, maker_fee_rate, ERR_MSG_PARAM_PAIR_MAKER_FEE_RATE_MUST_GE_ZERO));

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

   check(it == markets_.end(), ERR_MSG_CHECK_FAILED(50110, str_pair + ERR_MSG_PAIR_HAS_EXISTED)); //exchang pair has exist

   markets_.emplace(_self, [&](market &m) {
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
      m.zero_rate = zero_rate;
      m.cancel_ad_num = cancel_ad_num;
      m.pay_timeout = pay_timeout;
      m.self_playcoin_time = self_playcoin_time;
      m.def_playcoin_time = def_playcoin_timeout;
      m.def_cancel_time = def_cancel_timeout;
      m.status = MARKET_STATUS_ON;
      m.str_status = MARKET_STATUS_ON_STR;
      m.nickname = str_pair;
      m.ctime = time_point_sec(current_time_point().sec_since_epoch());
      m.utime = time_point_sec(current_time_point().sec_since_epoch());
   });
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

ACTION otcexchange::rmmarket(const symbol_code &pair)
{
   require_auth(_self); //必须要求是合约账号的权限+table是合约范围内的
   auto it = get_market(pair);
   markets_.erase(it);
   print("delete one market finish\n");
}

ACTION otcexchange::rmmarkets()
{
   require_auth(_self); //必须要求是合约账号的权限
   erase_markets();     //table是合约范围内的
   print("delete all market finish\n");
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

   //判断交易价格是否在交易对的配置区间
   check(price > itr_pair->zero_money, ERR_MSG_PARAM_LT_ZERO(50210, price, ERR_MSG_PARAM_AD_PRICE_MUST_GT_ZERO));
   check(price >= itr_pair->price_min, "广告交易价格小于合约配置最低价"); //提示用户卖出最低价不能小于XX
   check(price <= itr_pair->price_max, "广告交易价格大于合约配置最高价"); //提示用户买入最高价不能大于XX

   //判断交易数量是否在交易对配置区间
   check(amount > itr_pair->zero_stock, ERR_MSG_PARAM_LT_ZERO(50211, amount, ERR_MSG_PARAM_AD_AMOUNT_MUST_GT_ZERO));
   check(amount >= itr_pair->amount_min, "广告交易数量小于配置最小交易数量"); //提示用户最小交易数量不能小于XX
   check(amount <= itr_pair->amount_max, "广告交易数量大于配置最大交易数量"); //提示用户最大交易数量不能大于多少

   //判断吃单的交易限额是否合法
   check(amount_min > itr_pair->zero_stock, ERR_MSG_PARAM_LE_ZERO(50212, amount_min, ERR_MSG_PARAM_AD_AMOUNT_MIN_MUST_GT_ZERO));
   check(amount_min <= itr_pair->amount_min, "允许吃单的最小数量不能大于广告的最小委托数量");
   check(amount_max > itr_pair->zero_stock, ERR_MSG_PARAM_LE_ZERO(50213, amount_max, ERR_MSG_PARAM_AD_AMOUNT_MAX_MUST_GT_ZERO));
   check(amount_min <= amount_max, ERR_MSG_PARAM_LE(50215, amount_min, amount_max, ERR_MSG_PARAM_AD_AMOUNT_MAX_MUST_GE_MIN));
   check(amount_max <= amount, ERR_MSG_PARAM_LE(50216, amount_max, amount, ERR_MSG_PARAM_AD_AMOUNT_MUST_GE_AMOUNT_MAX));

   check((side == MARKET_ORDER_SIDE_ASK_STR || side == MARKET_ORDER_SIDE_BID_STR), ERR_MSG_SIDE);

   auto side_int = side_to_uint(side);
   //是卖，判断余额够不够
   if (side_int == MARKET_ORDER_SIDE_ASK)
   {
//如果冻结失败,提示可用余额不足
#ifdef ENV_DEV
      transfer(user, name{TOKEN_TEMP_ACCOUNT}, amount, std::string("广告卖币,需要冻结,把钱转到合约帐号:").append(amount.to_string()));
#else
      freeze_stock(user, amount, 1, std::string("广告卖币,需要冻结,把钱转到合约帐号:").append(amount.to_string()));
#endif
      print("是卖，可用余额不足"); //这里要发送一笔转账,也就是先要冻结
   }

   //要注意name类型的长度限制
   std::string scope = pair.to_string() + MARKET_ROLE_MAKER_STR + side;
   transform(scope.begin(), scope.end(), scope.begin(), ::tolower); //转成小写

   adorder_index_t adtable_(_self, name{scope}.value);

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

   print("广告上架成功");
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
      check((d->status == DEAL_STATUS_UNPAID_MAN_CANCEL ||
             d->status == DEAL_STATUS_UNPAID_TIMEOUT_CANCEL ||
             d->status == DEAL_STATUS_PAID_PLAYCOIN_FINISH),
            "有正在交易的taker deal,不能下架");
   }

   transfer(name{TOKEN_TEMP_ACCOUNT}, user, it->left, std::string("卖币广告手动下架,需要解冻,把钱转给用户:").append(it->left.to_string()));
   print("卖币广告手动下架,需要解冻,把钱转给用户ok");

   adtable_.modify(it, user, [&](adorder &ad) {
      ad.status = AD_STATUS_MAN_OFFTHESHELF; //手动下架
      ad.utime = current_time_point();
      ad.source.append("手动下架").append(reason);
   });

   print("广告下架成功");
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

   auto itr_pair = get_open_market(pair);

   check(price > itr_pair->zero_money, ERR_CHECK_PRICE_GREAT_ZERO);   //要求吃单价>0
   check(amount > itr_pair->zero_stock, ERR_CHECK_AMOUNT_GREAT_ZERO); //吃单数量>0

   //交易对的属性比对

   //校验side是否合法
   check((side == MARKET_ORDER_SIDE_ASK_STR || side == MARKET_ORDER_SIDE_BID_STR), ERR_MSG_SIDE);
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
   std::transform(scope_maker.begin(), scope_maker.end(), scope_maker.begin(), ::tolower); //scope要求是小写

   adorder_index_t adtable_(_self, name{scope_maker}.value);
   //要求挂单必须存在
   auto mk_it = adtable_.require_find(ad_id, MAKER_ORDER_NOR_EXIST_STR);

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

   //判断数量是不是合法

   if (tk_side == MARKET_ORDER_SIDE_BID)
   {
      check(price >= mk_it->price, "我出的买价必须>=卖价,才能撮合");
      print("对手方是卖币,他还有充足的币可以卖");
   }
   else
   {
      print("吃单卖币，可用余额足不足呢？");
      //我是卖币方,看我是不是还有币可卖,如果有,则直接冻结,这里发送一个内联调用action,如果执行成功，就继续，失败，就不往下走
      check(price <= mk_it->price, "我的卖价必须<=买价,才能撮合");
      //冻结我的币,提示用户您的余额不足
      transfer(user, name{TOKEN_TEMP_ACCOUNT}, amount, std::string("taker 卖币,需要冻结,把钱转到合约帐号:").append(amount.to_string()));
      print("我要卖币给广告方，可用余额够"); //这里要发送一笔转账,也就是先要冻结
   }

   //上面判断都通过，生成一个成交记录，也就是吃单详情,属于合约，scope是交易对

   std::string scope = pair.to_string();
   transform(scope.begin(), scope.end(), scope.begin(), ::tolower);

   deal_index_t dealtable_(_self, name{scope}.value);

   uint64_t deal_id = dealtable_.available_primary_key();
   auto now = current_time_point();

   dealtable_.emplace(user, [&](deal &deal) {
      deal.id = deal_id;
      deal.user = user;    //订单所属用户，注意是name类型
      deal.side = tk_side; //买卖类型，1卖 2买

      deal.type = MARKET_ORDER_TYPE_LIMIT; //订单类型，1限价 2市价
      deal.role = MARKET_ROLE_TAKER;       //订单类型，1挂单 2吃单
      deal.ctime = now;                    //订单创建时间，精确到微秒
      deal.utime = deal.ctime;             //订单更新时间，精确到微秒

      deal.maker_user = mk_user;
      deal.maker_order_id = ad_id;

      deal.price = price;   //订单交易价格
      deal.amount = amount; //订单交易数量

      deal.quota = asset(price.amount * amount.amount, itr_pair->money); //算一下交易额是多少,也就是买币方要支付的法币

      if (tk_side == MARKET_ORDER_SIDE_ASK)
      { //假如我是卖币
         deal.ask_fee = amount * (itr_pair->taker_fee_rate.amount);
         deal.bid_fee = amount * (itr_pair->maker_fee_rate.amount);
      }
      else
      {
         deal.ask_fee = amount * (itr_pair->maker_fee_rate.amount);
         deal.bid_fee = amount * (itr_pair->taker_fee_rate.amount);
      }

      deal.status = DEAL_STATUS_UNPAID; //待支付状态
      deal.pay_timeout = itr_pair->pay_timeout;
      deal.pay_timeout_sender_id = now.sec_since_epoch();
      deal.self_playcoin_sender_id = 0;
      deal.arbiarate_cancel_sender_id = 0;
      deal.arbiarate_playcoin_sender_id = 0;
      deal.pair = pair;
      deal.source = source;
   });

   //修改广告挂单的状态
   adtable_.modify(mk_it, mk_it->user, [&itr_pair, &deal_id, &amount, &tk_side](adorder &o) {
      o.vec_deal.emplace_back(deal_id);
      o.source.append("|").append("被吃单");
      o.utime = current_time_point();
      o.left = o.left - amount; //下单成功,减少广告单可交易数量
      //mk是卖币方
      o.freeze = o.freeze + amount; //o.freeze是在处于交易中的币正在等待对方法币支付
      otcexchange::update_ad_status(o, itr_pair);
   });

   //待支付状态,需要发送一个延时事物,法币支付超时自动取消,发送一个延迟事物;合约取消的

   transaction t{};

   name who = (tk_side == MARKET_ORDER_SIDE_ASK) ? mk_user : user;

   t.actions.emplace_back(
       permission_level(_self, name{"active"}),
       _self,
       name{"defcldeal"},
       std::make_tuple(pair, who, deal_id, DEAL_STATUS_UNPAID_TIMEOUT_CANCEL, "支付超时取消deal"));

   t.delay_sec = itr_pair->pay_timeout;

   t.send(now.sec_since_epoch(), _self);

   //print_f("Sent with a delay of %d ", t.delay_sec);
}

//法币支付发点击了已支付
//法币支付发点击了已支付
ACTION otcexchange::paydeal(const symbol_code &pair, uint64_t deal_id) //支付
{

   deal_index_t dealtable_(_self, name{pair.to_string()}.value);
   auto it = dealtable_.require_find(deal_id, "吃单找不到");
   auto user = it->user;
   require_auth(user);
   check(it->status == DEAL_STATUS_UNPAID, "订单必须处于待支付状态");
   dealtable_.modify(it, _self, [&](deal &d) {
      d.status = DEAL_STATUS_PAID_WAIT_PLAYCOIN;
      d.utime = current_time_point();
      d.source.append("|点击已支付");
   });
   //取消异步事物
   auto res = cancel_deferred(it->pay_timeout_sender_id);
   print(res, "->1 if transaction was canceled, 0 if transaction was not found");
}

//手动取消：买币方待支付->待支付取消完成
//超时取消：买币方待支付->超时待支付取消
//仲裁取消：仲裁中->仲裁取消中->取消完成
//终审取消：仲裁放币/取消->终审取消中->取消完成

void otcexchange::rollbackdeal_v1(const symbol_code &pair,
                                  name who,
                                  deal_index_t &dealtable_,
                                  deal_index_t::const_iterator itr_deal,
                                  uint8_t status,
                                  const std::string &reason)
{

   //入参校验
   check((status == DEAL_STATUS_UNPAID_TIMEOUT_CANCEL ||
          status == DEAL_STATUS_UNPAID_MAN_CANCEL ||
          status == DEAL_STATUS_CANCEL_FINISH),
         "入参：取消后的订单只能是未付款(超时自动/手动)和被已付款仲裁取消或者终审取消后的取消完成");

   //订单状态已经改变,请刷新后查看
   check((itr_deal->status == DEAL_STATUS_UNPAID ||
          //itr_deal->status == DEAL_STATUS_PAID_ARBIARATE_ING ||
          itr_deal->status == DEAL_STATUS_PAID_ARBIARATE_CANCEL ||
          itr_deal->status == DEAL_STATUS_PAID_JUDGE_CANCEL),
         "只有处于待支付状态或者仲裁中的订单或者对仲裁结果在进行仲裁才能取消");

   check(((itr_deal->status == DEAL_STATUS_UNPAID && (status == DEAL_STATUS_UNPAID_TIMEOUT_CANCEL || status == DEAL_STATUS_UNPAID_MAN_CANCEL)) ||
          (((itr_deal->status == DEAL_STATUS_PAID_ARBIARATE_CANCEL) || (itr_deal->status == DEAL_STATUS_PAID_JUDGE_CANCEL)) && (status == DEAL_STATUS_CANCEL_FINISH))),
         "deal 状态不匹配");

   auto itr_pair = get_market(pair);
   auto user = itr_deal->user;          //deal的用户
   auto mk_user = itr_deal->maker_user; //广告主

   //在待支付阶段要做这个判断，只有买币的一方才能取消,待支付手动取消和超时取消
   if (itr_deal->status == DEAL_STATUS_UNPAID)
   {
      if (mk_user == who)
      {
         //是广告主发起的买币才能进行取消操作
         check(itr_deal->side == MARKET_ORDER_SIDE_ASK, "广告主只能取自己的买币吃单");
      }
      else
      {

         check(itr_deal->side == MARKET_ORDER_SIDE_BID, "taker 只能取消买币吃单"); //卖币如果能取消,法币怎么退回呢?
      }
   }

   std::string mk_side("");
   if (itr_deal->side == MARKET_ORDER_SIDE_ASK)
   {
      mk_side = MARKET_ORDER_SIDE_BID_STR;
   }
   else
   {
      mk_side = MARKET_ORDER_SIDE_ASK_STR;
   }

   auto adtable_ = get_adtable(pair, mk_side);
   auto itr_ad = adtable_.require_find(itr_deal->maker_order_id, "对应的广告订单不存在,无法取消");

   //deal已经取消了,回滚广告单的处理
   adtable_.modify(itr_ad, _self, [&](adorder &ad) {
      ad.left = ad.left + itr_deal->amount;
      ad.freeze = ad.freeze - itr_deal->amount;
      ad.update_ad_status(itr_pair);
      //update_ad_status(ad, itr_pair);
   });

   //deal是买币的,就不用做什么操作了.
   //deal 表
   dealtable_.modify(itr_deal, _self, [&](deal &d) {
      d.status = status;
      d.utime = current_time_point();
      d.source.append(reason);

      //手动取消这些是不是重置为0
      //d.quota=itr_pair->zero_money;
      //d.ask_fee = itr_pair->zero_stock;
      //d.bid_fee = itr_pair->zero_stock;
   });
   //广告表
}

void otcexchange::rollbackdeal(const symbol_code &pair, name who, uint64_t deal_id, uint8_t status, const std::string &reason)
{

   deal_index_t dealtable_(_self, name{pair.to_string()}.value);
   auto it = dealtable_.require_find(deal_id, "deal id 不存在");
   rollbackdeal_v1(pair, who, dealtable_, it, status, reason);
}

void otcexchange::commitdeal(const symbol_code &pair, name who, uint64_t deal_id, uint8_t status, const std::string &reason) //放币操作
{

   deal_index_t dealtable_(_self, name{pair.to_string()}.value);
   auto it = dealtable_.require_find(deal_id, "deal id 不存在");
   commitdeal_v1(pair, who, dealtable_, it, status, reason);
}

//自己放币：已付款待支付中->放币中->放币完成
//仲裁放币：仲裁中->仲裁放币中->放币完成
//终审放币：仲裁取消或放币->终审放币中->放币完成

void otcexchange::commitdeal_v1(const symbol_code &pair,
                                name who,
                                deal_index_t &dealtable_,
                                deal_index_t::const_iterator itr_deal,
                                uint8_t status,
                                const std::string &reason)
{

   auto user = itr_deal->user;

   check(status == DEAL_STATUS_PAID_PLAYCOIN_FINISH, "入参必须是放币完成");

   check(itr_deal->status == DEAL_STATUS_PAID_PLAYCOIN_ING ||
             itr_deal->status == DEAL_STATUS_PAID_ARBIARATE_PALYCOIN ||
             itr_deal->status == DEAL_STATUS_PAID_JUDGE_PLAYCOIN,
         "订单必须处于自己放币中和初审放币和终审放币状态");

   name from;
   name to;
   if (itr_deal->side == MARKET_ORDER_SIDE_ASK) //是卖币是taker
   {
      /* code */
      from = itr_deal->user;
      to = itr_deal->maker_user;
   }
   else
   {
      to = itr_deal->user;
      from = itr_deal->maker_user;
   }
   check(who == from, "放币的人搞错了");

   std::string str_mk_side = (itr_deal->side == MARKET_ORDER_SIDE_ASK) ? MARKET_ORDER_SIDE_BID_STR : MARKET_ORDER_SIDE_ASK_STR;
   auto adtable = get_adtable(pair, str_mk_side);

   auto itr_ad = adtable.require_find(itr_deal->maker_order_id, "ad order not exist");

   //发一个转账action

   transfer(from, to, itr_deal->amount, reason); //放币
   transfer(from, to, itr_deal->amount, reason); //fee

   print("放币了,把币转给用户ok");

   //修改deal的状态
   dealtable_.modify(itr_deal, user, [&](deal &d) {
      d.status = status; //放币中
      d.utime = current_time_point();
      d.source.append(reason);
   });

   auto itr_pair = get_market(pair);

   //修改广告的状态
   adtable.modify(itr_ad, _self, [&](adorder &ad) {
      ad.utime = current_time_point();
      ad.source.append(reason);
      ad.freeze = ad.freeze - itr_deal->amount;
      update_ad_status(ad, itr_pair);
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