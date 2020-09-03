#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/action.hpp>
#include <eosio/transaction.hpp>
#include <eosio/singleton.hpp>
#include <string>
#define USING_ACTION(name) using name##_action = action_wrapper<#name##_n, &maker::name>

using namespace eosio;


CONTRACT maker : public contract {
   public:
      using contract::contract;

      ACTION hi( name nm );
      ACTION wrapperhi(name nm);
      ACTION insert(name account,std::string tip);
      ACTION erase();
      ACTION update();
      ACTION query();
      ACTION getbyaccount(name account);
      ACTION sendtoken(name from, name to);
      ACTION defered(  name from, const std::string& msg);

     
      USING_ACTION(hi);
      USING_ACTION(insert);
      USING_ACTION(erase);
      USING_ACTION(update);
      USING_ACTION(query);
      USING_ACTION(getbyaccount);
      USING_ACTION(sendtoken);
      USING_ACTION(defered);
   public:
      [[eosio::on_notify("eosio.token::transfer")]] 
      void on_transfer(name from,name to,asset quantity,std::string msg);

      [[eosio::on_notify("eosio::onerror")]] 
      void onError(const onerror &error);

   private:
      TABLE order_t{
         uint64_t       id;            //自增id 
         name           account;       //用户名
         time_point_sec ctime;         //创建时间
         time_point_sec utime;         //更新时间
         std::string    tip;           //备注信息
         uint64_t primary_key() const {return id; }
         uint64_t get_secondary_account() const {return account.value;}

      };

      using order_index_t = multi_index<"order"_n,order_t,
      indexed_by<"byaccount"_n,const_mem_fun<order_t,uint64_t,&order_t::get_secondary_account>>
      > ;


};