#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
using namespace eosio;

CONTRACT otcsetting : public contract {
   public:
      using contract::contract;

      ACTION hi( name nm );

      using hi_action = action_wrapper<"hi"_n, &otcsetting::hi>;

   private:
      TABLE gobal_setting{
         uint64_t curuserid;
         std::string msg;

      };
      using singleton_t =singleton<"gobal_setting"_n,gobal_setting>;
      singleton_t otc_conf;


   
};