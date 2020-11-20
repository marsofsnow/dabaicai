#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>
#include <eosio/singleton.hpp>
#include <eosio/dispatcher.hpp>
#include <eosio/crypto.hpp>
#include <eosio/action.hpp>

#include <string>



namespace otcsystem
{

    /******************************************* common *******************************************/
    using std::for_each;
    using std::string;

    using eosio::action;
    using eosio::asset;
    using eosio::block_timestamp;
    using eosio::check;
    using eosio::checksum256;
    using eosio::const_mem_fun;
    using eosio::contract;
    using eosio::current_time_point;
    using eosio::datastream;
    using eosio::indexed_by;
    using eosio::name;
    using eosio::permission_level;
    using eosio::same_payer;
    using eosio::sha256;
    using eosio::symbol;
    using eosio::symbol_code;
    using eosio::time_point;
    using eosio::time_point_sec;
    using eosio::unsigned_int;

    static constexpr name active_permission{"active"_n};
    static constexpr name transfer_act{"transfer"_n};
    static constexpr name canmodpay_act{"canmodpay"_n};
    static constexpr name token_account{"adxio.token"_n};
    static constexpr name otcexch_account{"otcexchange"_n};
    static constexpr name otcsys_account{"otcsystem"_n};

    // 账号类型
    static constexpr uint8_t account_type_otc = 1; // OTC账号
    static constexpr uint8_t account_type_abt = 2; // 仲裁账号

    // 用户类型
    static constexpr uint8_t user_type_normal = 0;   // 普通用户
    static constexpr uint8_t user_type_admin = 1;    // 佣金管理员
    static constexpr uint8_t user_type_applying = 2; // 佣金管理员申请中

    // 收款方式类型
    static constexpr uint32_t payment_type_bank = 0x00000001;   // 银行
    static constexpr uint32_t payment_type_alipay = 0x00000002; // 支付宝
    static constexpr uint32_t payment_type_wechat = 0x00000004; // 微信
    static constexpr uint32_t payment_type_paypal = 0x00000008; // paypal
    static constexpr uint32_t payment_type_other = 0x80000000;  // other

    // 消息类型
    static constexpr uint8_t msg_type_text = 1;        // 文本消息
    static constexpr uint8_t msg_type_request_pmt = 2; // 请求收款方式消息
    static constexpr uint8_t msg_type_reply_pmt = 3;   // 回复收款方式消息
    static constexpr uint8_t msg_type_appeal = 4;      // 申诉消息

    // 返佣
    static constexpr uint8_t apply_state_applying = 0; // 申请中
    static constexpr uint8_t apply_state_approved = 1; // 已批准
    static constexpr uint8_t apply_state_reject = 2;   // 已拒绝

    // 财务流水类型
    static constexpr uint8_t trx_type_freeze = 0x01;   // 广告卖币冻结、解冻
    static constexpr uint8_t trx_type_stake = 0x02;    // 仲裁质押、解除质押
    static constexpr uint8_t trx_type_transfer = 0x03; // 买币、卖币
    static constexpr uint8_t trx_type_fee = 0x04;      // 手续费
    static constexpr uint8_t trx_type_bonus = 0x05;    // 仲裁奖金
    static constexpr uint8_t trx_type_rebate = 0x06;   // 返佣

    // ontransfer函数参数
    struct transfer_t
    {
        name from;
        name to;
        asset quantity;
        string memo;

        EOSLIB_SERIALIZE(transfer_t, (from)(to)(quantity)(memo))
    };

    /******************************************** sys ********************************************/
    struct [[eosio::table("sysconfig"), eosio::contract("otcsystem")]] sys_config
    {
        uint32_t signin_expiration;         // 登陆超时时间
        uint32_t rebate_max_level;          // 返佣团队默认最大级数
        uint32_t default_percent_precision; // 百分比精度
        uint32_t default_forex_precision;   // 外汇汇率精度
        uint32_t default_rebate_rate;       // 交易佣金最大返还比例，默认50%
        uint32_t default_referral_rate;     // 直属下线分配占比，范围0-100%，默认50%
        uint32_t default_price_limit;       // 价格偏离限制，默认10%
        uint32_t default_fee_rate;          // 交易费率，默认2%

        sys_config() : signin_expiration(3600 * 24 * 7),
                       rebate_max_level(5),
                       default_percent_precision(10000),
                       default_forex_precision(10000),
                       default_rebate_rate(5000),
                       default_referral_rate(5000),
                       default_price_limit(1000),
                       default_fee_rate(200)
        {
        }

        EOSLIB_SERIALIZE(sys_config,
                         (signin_expiration)(rebate_max_level)(default_percent_precision)(default_forex_precision)(default_rebate_rate)(default_referral_rate)(default_price_limit)(default_fee_rate))
    };

    struct [[eosio::table("sysstats"), eosio::contract("otcsystem")]] sys_stats
    {
        uint32_t user_count = 0; // 总用户数
        uint32_t blk_count = 0;  // 黑名单人数

        EOSLIB_SERIALIZE(sys_stats, (user_count)(blk_count))
    };

    struct [[eosio::table, eosio::contract("otcsystem")]] user_stats
    {
        name owner;                  // 存款人
        uint32_t signin_count = 0;   // 登陆次数
        uint32_t deposit_count = 0;  // 存款次数
        uint32_t withdraw_count = 0; // 提现次数

        uint64_t primary_key() const { return owner.value; }

        EOSLIB_SERIALIZE(user_stats, (owner)(signin_count)(deposit_count)(withdraw_count))
    };

    typedef eosio::singleton<"sysconfig"_n, sys_config> sys_config_singleton;
    typedef eosio::singleton<"sysstats"_n, sys_stats> sys_stats_singleton;
    typedef eosio::multi_index<"usrstats"_n, user_stats> user_stats_table;

    /******************************************* asset *******************************************/
    struct [[eosio::table, eosio::contract("otcsystem")]] sys_account
    {
        asset total_deposit;  // 总存款，收入
        asset total_withdraw; // 总提现，支出
        asset total_transfer; // 总转账，支出
        asset total_fee;      // 总手续费，记账
        asset total_rebate;   // 总返佣，从fee中支出
        asset total_bonus;    // 总仲裁奖励，从fee中支出

        asset balance;        // 总余额
        asset frozen_balance; // 总冻结余额
        asset fee_balance;    // 总手续费余额

        uint64_t primary_key() const { return total_deposit.symbol.code().raw(); }
        EOSLIB_SERIALIZE(sys_account, (total_deposit)(total_withdraw)(total_transfer)(total_fee)(total_rebate)(total_bonus)(balance)(frozen_balance)(fee_balance))
    };

    struct [[eosio::table, eosio::contract("otcsystem")]] account
    {
        asset balance;   // 总余额
        asset frozen;    // 冻结余额
        asset fee_bonus; // 总赚取手续费佣金
        asset abt_bonus; // 总赚取仲裁佣金

        asset available() const { return balance - frozen; } // 可用的
        uint64_t primary_key() const { return balance.symbol.code().raw(); }

        EOSLIB_SERIALIZE(account, (balance)(frozen)(fee_bonus)(abt_bonus))
    };

    struct [[eosio::table, eosio::contract("otcsystem")]] transact
    {
        uint64_t trx_id; // ID
        name from;       // 转出账号
        name to;         // 转入账号
        asset quantity;  // 数额
        string memo;     // 备注

        uint64_t trx_type = 0;    // 具体操作类型
        time_point trx_time;      // 交易时间
        uint32_t rebate_rate = 0; // 返佣比率(只在返佣时用到）

        uint64_t primary_key() const { return trx_id; }
        uint64_t by_symbol() const { return quantity.symbol.code().raw(); }
        uint64_t by_time() const { return trx_time.elapsed.count(); }

        EOSLIB_SERIALIZE(transact, (trx_id)(from)(to)(quantity)(memo)(trx_type)(trx_time)(rebate_rate))
    };

    typedef eosio::multi_index<"sysaccount"_n, sys_account> sys_account_table;
    typedef eosio::multi_index<"accounts"_n, account> account_table;

    // OTC财务流水
    typedef eosio::multi_index<"trxotcs"_n, transact,
                               indexed_by<"bysymbol"_n, const_mem_fun<transact, uint64_t, &transact::by_symbol>>,
                               indexed_by<"bytime"_n, const_mem_fun<transact, uint64_t, &transact::by_time>>>
        trxotc_table;
    // 仲裁财务流水
    typedef eosio::multi_index<"trxabts"_n, transact,
                               indexed_by<"bysymbol"_n, const_mem_fun<transact, uint64_t, &transact::by_symbol>>,
                               indexed_by<"bytime"_n, const_mem_fun<transact, uint64_t, &transact::by_time>>>
        trxabt_table;
    // 返佣财务流水
    typedef eosio::multi_index<"trxrbts"_n, transact,
                               indexed_by<"bysymbol"_n, const_mem_fun<transact, uint64_t, &transact::by_symbol>>,
                               indexed_by<"bytime"_n, const_mem_fun<transact, uint64_t, &transact::by_time>>>
        trxrbt_table;

    // 冻结财务流水
    typedef eosio::multi_index<"trxfrzs"_n, transact,
                               indexed_by<"bysymbol"_n, const_mem_fun<transact, uint64_t, &transact::by_symbol>>,
                               indexed_by<"bytime"_n, const_mem_fun<transact, uint64_t, &transact::by_time>>>
        trxfrz_table;

    /******************************************* blacklist *******************************************/
    struct [[eosio::table, eosio::contract("otcsystem")]] blacklist
    {
        uint64_t blk_id;      //ID
        checksum256 blk_hash; //hash

        uint64_t primary_key() const { return blk_id; }
        checksum256 by_hash() const { return blk_hash; }

        EOSLIB_SERIALIZE(blacklist, (blk_id)(blk_hash))
    };

    struct [[eosio::table, eosio::contract("otcsystem")]] identity
    {
        name account;          //账号
        checksum256 ac_hash;   //账号hash
        checksum256 id_hash;   //app标识ID
        checksum256 ip_hash;   //IP
        time_point expiration; //过期时间

        uint64_t primary_key() const { return account.value; }

        EOSLIB_SERIALIZE(identity, (account)(ac_hash)(id_hash)(ip_hash)(expiration))
    };

    typedef eosio::multi_index<"blacklists"_n, blacklist> blacklist_table;
    typedef eosio::multi_index<"identitys"_n, identity> identity_table;

    /******************************************** message ********************************************/
    struct [[eosio::table, eosio::contract("otcsystem")]] message
    {
        uint64_t msg_id;   // 消息ID
        name sender;       // 发送账号
        name receiver;     // 接收账号
        string ciphertext; // 消息密文

        uint64_t deal_id;    // 成交订单ID
        uint64_t msg_type;   // 消息类型 1:普通文本消息 2:买方请求付款方式消息 3:卖方发送付款方式消息
        time_point msg_time; // 发送时间

        uint64_t primary_key() const { return msg_id; }
        uint64_t by_deal() const { return deal_id; }
        uint64_t by_type() const { return msg_type; }
        uint64_t by_time() const { return msg_time.elapsed.count(); }

        EOSLIB_SERIALIZE(message, (msg_id)(sender)(receiver)(ciphertext)(deal_id)(msg_type)(msg_time))
    };

    typedef eosio::multi_index<"messages"_n, message,
                               indexed_by<"bydeal"_n, const_mem_fun<message, uint64_t, &message::by_deal>>,
                               indexed_by<"bytype"_n, const_mem_fun<message, uint64_t, &message::by_type>>,
                               indexed_by<"bytime"_n, const_mem_fun<message, uint64_t, &message::by_time>>>
        message_table;

    /******************************************** payment ********************************************/
    struct [[eosio::table, eosio::contract("otcsystem")]] payment
    {
        uint64_t pmt_id;        // 支付方式ID
        uint32_t pmt_type = 0;  // 支付方式类型
        bool is_actived = true; // 是否激活
        string ciphertext;      // 付款方式密文

        uint64_t primary_key() const { return pmt_id; }
        uint64_t by_type() const { return pmt_type; }
        uint64_t by_actived() const { return is_actived; }

        EOSLIB_SERIALIZE(payment, (pmt_id)(pmt_type)(is_actived)(ciphertext))
    };

    typedef eosio::multi_index<"payments"_n, payment,
                               indexed_by<"bytype"_n, const_mem_fun<payment, uint64_t, &payment::by_type>>,
                               indexed_by<"byactived"_n, const_mem_fun<payment, uint64_t, &payment::by_actived>>>
        payment_table;

    /******************************************** rebate ********************************************/
    struct [[eosio::table, eosio::contract("otcsystem")]] user
    {
        name owner;                           // 账号
        name admin;                           // 团队管理员账号
        string nickname;                      // 昵称
        uint8_t user_type = user_type_normal; // 0: 普通用户 1: 返佣管理员
        uint32_t pmts_type = 0;               // 收款方式
        time_point signup_time;               // 注册时间

        name inviter;       // 邀请人
        string invite_code; // 邀请码

        uint32_t rebate_rate = 0;   // 返佣比率
        uint32_t referral_rate = 0; // 下线返佣比率

        uint32_t direct_referrals = 0; // 直接下线人数
        uint32_t total_referrals = 0;  // 总下线人数

        uint64_t primary_key() const { return owner.value; }
        uint64_t by_inviter() const { return inviter.value; }
        uint64_t by_admin() const { return admin.value; }
        uint64_t by_type() const { return user_type; }

        EOSLIB_SERIALIZE(user, (owner)(admin)(nickname)(user_type)(pmts_type)(signup_time)(inviter)(invite_code)(rebate_rate)(referral_rate)(direct_referrals)(total_referrals))
    };

    struct [[eosio::table, eosio::contract("otcsystem")]] team
    {
        symbol symbol;                  // 代币符号
        uint32_t maker_ask = 0;         // 挂单卖币费率
        uint32_t maker_bid = 0;         // 挂单买币费率
        uint32_t taker_ask = 0;         // 吃单卖币费率
        uint32_t taker_bid = 0;         // 吃单买币费率
        uint32_t price_limit_upper = 0; // 价格上限偏离值
        uint32_t price_limit_lower = 0; // 价格下限偏离值

        uint64_t primary_key() const { return symbol.code().raw(); }

        EOSLIB_SERIALIZE(team, (symbol)(maker_ask)(maker_bid)(taker_ask)(taker_bid)(price_limit_upper)(price_limit_lower))
    };

    struct [[eosio::table, eosio::contract("otcsystem")]] apply_t
    {
        name owner;               // 账号
        uint32_t rebate_rate = 0; // 申请返佣比例
        uint32_t team_users = 0;  // 预计团队人数
        string prefix_nickname;   // 昵称前辍

        uint64_t state = apply_state_applying; // 0: 申请中 1: 已批准 2: 已拒绝
        string request;                        // 申请说明
        string reply;                          // 批复备注
        time_point apply_time;                 // 申请时间
        time_point approve_time;               // 批复时间

        uint64_t primary_key() const { return owner.value; }
        uint64_t by_state() const { return state; }

        EOSLIB_SERIALIZE(apply_t, (owner)(rebate_rate)(team_users)(prefix_nickname)(state)(request)(reply)(apply_time)(approve_time))
    };

    typedef eosio::multi_index<"users"_n, user,
                               indexed_by<"byinviter"_n, const_mem_fun<user, uint64_t, &user::by_inviter>>,
                               indexed_by<"byadmin"_n, const_mem_fun<user, uint64_t, &user::by_admin>>,
                               indexed_by<"bytype"_n, const_mem_fun<user, uint64_t, &user::by_type>>>
        user_table;

    typedef eosio::multi_index<"teams"_n, team> team_table;

    typedef eosio::multi_index<"applys"_n, apply_t,
                               indexed_by<"bystate"_n, const_mem_fun<apply_t, uint64_t, &apply_t::by_state>>>
        apply_table;

    /******************************************* oracle *******************************************/
    struct [[eosio::table, eosio::contract("otcsystem")]] forexrate
    {
        symbol_code money;      // 法币符号
        uint64_t boc_rate;      // 中行折算价(美元兑换汇率)
        time_point update_time; // 更新时间

        uint64_t primary_key() const { return money.raw(); }

        EOSLIB_SERIALIZE(forexrate, (money)(boc_rate)(update_time))
    };

    struct [[eosio::table, eosio::contract("otcsystem")]] market
    {
        symbol_code pair;       // 交易对
        symbol_code money;      // 法币符号
        symbol_code stock;      // 代币符号
        uint64_t price;         // 代币价格
        time_point update_time; // 更新时间

        uint64_t primary_key() const { return pair.raw(); }

        EOSLIB_SERIALIZE(market, (pair)(money)(stock)(price)(update_time))
    };

    typedef eosio::multi_index<"forexrates"_n, forexrate> forexrate_table;
    typedef eosio::multi_index<"markets"_n, market> market_table;

    /******************************************* action *******************************************/
    struct [[eosio::table, eosio::contract("otcsystem")]] rfrlrate
    {
        name referral; // 下线账号
        double rate;   // 返佣比例

        EOSLIB_SERIALIZE(rfrlrate, (referral)(rate))
    };

    typedef std::vector<rfrlrate> vtrfrlrate;

    /******************************************* otcsystem *******************************************/
    class [[eosio::contract("otcsystem")]] otcsystem : public contract
    {
    private:
        sys_config_singleton _sys_config_st;
        sys_stats_singleton _sys_stats_st;
        sys_config _sys_config;
        sys_stats _sys_stats;
        user_stats_table _user_stats;
        sys_account_table _sys_account;

        user_table _users;
        blacklist_table _blacklist;
        identity_table _identity;
        apply_table _applys;

        forexrate_table _forexrates;
        market_table _markets;

        // only for test
        template <typename T>
        void erase_table(T & table);

        void open_account(const name &owner, const symbol &symbol);
        void sub_balance(const name &owner, const asset &amount);
        void add_balance(const name &owner, const asset &amount);
        void freeze_balance(const name &owner, const asset &quantity);

        string get_memo(uint8_t trx_type, bool positive = true);
        template <typename T>
        void save_transact(T & table, const name &from, const name &to, const asset &quantity, uint8_t trx_type, uint32_t rebate_rate = 0);
        void transfer_from_sys(const name &from, const name &to, const asset &quantity, uint8_t trx_type, uint32_t rebate_rate = 0);
        void transfer_to_sys(const name &from, const asset &quantity, uint8_t trx_type);

        void check_blacklist(const identity &idt);
        void check_identity(const name &account);
        void check_inviter(const name &owner, const name &inviter);
        void check_nickname(const name &user, const string &nickname);
        string create_random(const name &owner);
        bool is_supported(uint32_t payment_type);

        inline uint32_t get_defaut_referral_rate(uint32_t rebate_rate) { return rebate_rate * _sys_config.default_referral_rate / _sys_config.default_percent_precision; };
        inline asset calc_rebate(uint32_t rebate_rate, const asset &fee) { return rebate_rate * fee / _sys_config.default_percent_precision; };
        bool is_referral(const name &owner, const name &inviter);

        const name get_admin(const name &owner);
        string get_prefix(const name &admin);
        uint32_t get_referral_rate(const name &inviter);
        uint32_t get_max_referral_rate(const name &inviter);

        void check_levels(const name &inviter);
        void check_prefix(const string &prefix_nickname);

        void add_team_symbol(const name &admin, const symbol &symbol);

        void update_referrals_out(const name &proposer);
        void update_referrals_in(const name &owner, const name &inviter);
        void update_user_in(const name &owner, const name &admin, const string &prefix, uint32_t rebate_rate);
        void update_user_out(const name &owner, const name &admin, const string &old_prefix, const string &new_prefix);
        void update_team_in(const name &owner, const name &inviter);
        void update_team_out(const name &owner);

        asset pay_rebate(const name &payer, const asset &fee);

    public:
        void ontransfer(const struct transfer_t &trans);

        otcsystem(name s, name code, datastream<const char *> ds);
        ~otcsystem();

        // SYS
        [[eosio::action]] void cleartable(const name &table);
        [[eosio::action]] void clearalltable();
        [[eosio::action]] void setpkgurl(const string &pkg_url);

        // 资产
        [[eosio::action]] void open(const name &owner, const symbol_code &sym_code);
        [[eosio::action]] void deposit(const name &owner, const asset &quantity, uint8_t type);
        [[eosio::action]] void withdraw(const name &owner, const asset &quantity, uint8_t type);
        [[eosio::action]] void transfer(const name &owner, const name &to, const asset &quantity, const asset &fee, bool receiver);
        [[eosio::action]] void freeze(const name &owner, const asset &quantity, uint8_t type);
        [[eosio::action]] void award(const name &arbitrator, const asset &quantity);

        // 黑名单
        [[eosio::action]] void signup(const name &owner, const name &inviter, const string &nickname);
        [[eosio::action]] void signin(const name &owner, const checksum256 &act, const checksum256 &id, const checksum256 &ip);
        [[eosio::action]] void signout(const name &owner);
        [[eosio::action]] void modnickname(const name &owner, const string &nickname);
        [[eosio::action]] void addinviter(const name &owner, const name &inviter);
        [[eosio::action]] void addblacklist(const name &account);
        [[eosio::action]] void delblacklist(const name &account);
        [[eosio::action]] void chkblacklist(const name &account);

        // 消息
        [[eosio::action]] void sendmsg(uint64_t deal_id, uint8_t msg_type, const name &sender, const name &receiver, const string &msg_ciphertext);
        [[eosio::action]] void clearmsg(uint64_t deal_id, const name &account);

        // 收款方式
        [[eosio::action]] void addpayment(const name &owner, uint32_t pmt_type, bool is_actived, const string &pmt_ciphertext);
        [[eosio::action]] void delpayment(const name &owner, uint32_t pmt_id);
        [[eosio::action]] void modpayment(const name &owner, uint32_t pmt_id, bool is_actived, const string &pmt_ciphertext);

        // 返佣
        [[eosio::action]] void apply(const name &owner, double rebate_rate, uint32_t team_users, const string &prefix_nickname, const string &comment);
        [[eosio::action]] void approve(const name &proposer, uint8_t result, const string &comment);
        [[eosio::action]] void moddefrate(const name &owner, double rebate_rate);
        [[eosio::action]] void modrfrlrate(const name &owner, const name &referral, double rebate_rate);
        [[eosio::action]] void modrfrlrates(const name &owner, const vtrfrlrate &vfr);
        [[eosio::action]] void modpricelmt(const name &owner, const symbol_code &sym_code, double price_limit_upper, double price_limit_lower);
        [[eosio::action]] void modfeerate(const name &owner, const symbol_code &sym_code, double maker_ask, double maker_bid);

        // 汇率
        [[eosio::action]] void updaterate(const symbol_code &money, double boc_rate);
        [[eosio::action]] void deleterate(const symbol_code &money);

        using cleartable_action = eosio::action_wrapper<"cleartable"_n, &otcsystem::cleartable>;
        using clearalltable_action = eosio::action_wrapper<"clearalltable"_n, &otcsystem::clearalltable>;
        using setpkgurl_action = eosio::action_wrapper<"setpkgurl"_n, &otcsystem::setpkgurl>;
        using open_action = eosio::action_wrapper<"open"_n, &otcsystem::open>;
        using deposit_action = eosio::action_wrapper<"deposit"_n, &otcsystem::deposit>;
        using withdraw_action = eosio::action_wrapper<"withdraw"_n, &otcsystem::withdraw>;
        using transfer_action = eosio::action_wrapper<"transfer"_n, &otcsystem::transfer>;
        using freeze_action = eosio::action_wrapper<"freeze"_n, &otcsystem::freeze>;
        using award_action = eosio::action_wrapper<"award"_n, &otcsystem::award>;
        using signup_action = eosio::action_wrapper<"signup"_n, &otcsystem::signup>;
        using signin_action = eosio::action_wrapper<"signin"_n, &otcsystem::signin>;
        using signout_action = eosio::action_wrapper<"signout"_n, &otcsystem::signout>;
        using modnickname_action = eosio::action_wrapper<"modnickname"_n, &otcsystem::modnickname>;
        using addinviter_action = eosio::action_wrapper<"addinviter"_n, &otcsystem::addinviter>;
        using addblacklist_action = eosio::action_wrapper<"addblacklist"_n, &otcsystem::addblacklist>;
        using delblacklist_action = eosio::action_wrapper<"delblacklist"_n, &otcsystem::delblacklist>;
        using chkblacklist_action = eosio::action_wrapper<"chkblacklist"_n, &otcsystem::chkblacklist>;
        using sendmsg_action = eosio::action_wrapper<"sendmsg"_n, &otcsystem::sendmsg>;
        using clearmsg_action = eosio::action_wrapper<"clearmsg"_n, &otcsystem::clearmsg>;
        using addpayment_action = eosio::action_wrapper<"addpayment"_n, &otcsystem::addpayment>;
        using delpayment_action = eosio::action_wrapper<"delpayment"_n, &otcsystem::delpayment>;
        using modpayment_action = eosio::action_wrapper<"modpayment"_n, &otcsystem::modpayment>;
        using apply_action = eosio::action_wrapper<"apply"_n, &otcsystem::apply>;
        using approve_action = eosio::action_wrapper<"approve"_n, &otcsystem::approve>;
        using modselfrate_action = eosio::action_wrapper<"moddefrate"_n, &otcsystem::moddefrate>;
        using modrfrlrate_action = eosio::action_wrapper<"modrfrlrate"_n, &otcsystem::modrfrlrate>;
        using modrfrlrates_action = eosio::action_wrapper<"modrfrlrates"_n, &otcsystem::modrfrlrates>;
        using modpricelmt_action = eosio::action_wrapper<"modpricelmt"_n, &otcsystem::modpricelmt>;
        using modfeerate_action = eosio::action_wrapper<"modfeerate"_n, &otcsystem::modfeerate>;
        using updaterate_action = eosio::action_wrapper<"updaterate"_n, &otcsystem::updaterate>;
        using deleterate_action = eosio::action_wrapper<"deleterate"_n, &otcsystem::deleterate>;
    };
} // namespace otcsystem
