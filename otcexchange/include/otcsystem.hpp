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

    // 账号类型
    static constexpr uint8_t account_type_otc = 1; // OTC账号
    static constexpr uint8_t account_type_abt = 2; // 仲裁账号

    // 用户类型
    static constexpr uint8_t user_type_normal = 0; // 普通用户
    static constexpr uint8_t user_type_admin = 1;  // 佣金管理员

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

    typedef eosio::multi_index<"users"_n, user,
                               indexed_by<"byinviter"_n, const_mem_fun<user, uint64_t, &user::by_inviter>>,
                               indexed_by<"bytype"_n, const_mem_fun<user, uint64_t, &user::by_type>>>
        user_table;

    typedef eosio::multi_index<"teams"_n, team> team_table;

    /******************************************* otcsystem *******************************************/
    class [[eosio::contract("otcsystem")]] otcsystem : public contract
    {
    private:
        user_table _users;
        blacklist_table _blacklist;
        identity_table _identity;
    };
} // namespace otcsystem
