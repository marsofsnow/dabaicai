
#include <eosio/eosio.hpp>
#include <string>

using namespace eosio; 

CONTRACT learn : public contract {
   public:
      using contract::contract;

      ACTION hi( name nm );
      ACTION testcheck(){
         uint8_t a=1;
         uint8_t b=2;
         //check(a==b,"a !=b");
         print("1111",22,'\n');
         print_f("%->%\n","周浩",10);
         check(a==b,1);
         std::string name="刘清";
         print(name); //为什么没有输出呢？
      }

      using hi_action = action_wrapper<"hi"_n, &learn::hi>;
      using testcheck_action = action_wrapper<"testcheck"_n, &learn::testcheck>;
};


