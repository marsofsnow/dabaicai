
#include <eosio/eosio.hpp>
#include <string>
#include <vector>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>
#include <eosio/transaction.hpp>
#define USING_ACTION(name) using name##_action = action_wrapper<#name##_n, &learn::name>




using namespace eosio; 

CONTRACT learn : public contract {
   public:
      using contract::contract;

      
      ACTION hi( name nm );
      ACTION adduser(name account,uint64_t phone,std::string signture);
      ACTION edituser(name account,uint64_t phone,std::string signature);
      ACTION deleteuser(name account);
      ACTION loweruser(name account);
      ACTION addpost(name account,std::string content);

      //action的通信
      ACTION sendtoken(name from,name to);

      //注册一个action的监控函数，监听eosio.token::transfer这个action
      [[eosio::on_notify("eosio.token::transfer")]] 
      void onTransfer(name from, name to, asset quantity, std::string memo);

      //延迟ACTION
      ACTION defered(name from, const std::string& msg);
      ACTION send(name from,const std::string& msg,uint64_t delay);
      [[eosio::on_notify("eosio::onerror")]]
      void onError(const onerror& error);

      


      //using hi_action = action_wrapper<"hi"_n, &learn::hi>;
      //using testcheck_action = action_wrapper<"testcheck"_n, &learn::testcheck>;
      //using testlearn_action = action_wrapper<"testlearn"_n, &learn::testlearn>;
      //using testcontract_action = action_wrapper<"testcontract"_n, &learn::testcontract>;

      USING_ACTION(sendtoken);
      USING_ACTION(addpost);
      USING_ACTION(adduser);
      USING_ACTION(edituser);
      USING_ACTION(deleteuser);
      USING_ACTION(loweruser);
      USING_ACTION(hi);

   private:
      TABLE user {
         name account; //使用EOS账户名作为主键
         uint64_t phone; //手机号
         time_point_sec regtime; //注册时间
         std::string signature; //备注
         uint64_t primary_key() const { return account.value; }
         uint64_t get_secondary_1() const { return phone; }
         uint64_t get_secondary_2() const { return regtime.sec_since_epoch(); }
      };

      TABLE post { //文章
      uint64_t id; //使用自增id作为主键
      name author; //作者
      std::string content;//文章内容
      uint64_t primary_key() const { return id; }
      uint64_t get_secondary_1() const { return author.value; }
      };

      typedef multi_index<"user"_n, user, 
      indexed_by<"byphone"_n,    const_mem_fun<user, uint64_t, &user::get_secondary_1>>,
      indexed_by<"byregtime"_n,  const_mem_fun<user, uint64_t, &user::get_secondary_2>>
      > user_index;

      typedef multi_index<"post"_n, post,
      indexed_by<"byauthor"_n, const_mem_fun<post, uint64_t, &post::get_secondary_1>>
      > post_index;


      
};


