
#include <eosio/eosio.hpp>
#include <string>
#include <vector>
#include <eosio/system.hpp>
#define USING_ACTION(name) using name##_action = action_wrapper<#name##_n, &learn::name>




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
         std::vector<int> v{1,2,3,4,5};
         for(auto& item:v){
            print(item,'\n');

         }
         
      }
      ACTION testlearn(){
         print(seconds(11).count(),'\n');
         print(current_time_point().sec_since_epoch(), " ");
         print(current_block_time().to_time_point().sec_since_epoch(), " ");

      }

      ACTION testcontract(name account, uint64_t age) {
         print(get_self(), " ");            
         print(get_first_receiver(), " ");  
         print(account, " ");               
         print(age, " ");
      }


      //using hi_action = action_wrapper<"hi"_n, &learn::hi>;
      //using testcheck_action = action_wrapper<"testcheck"_n, &learn::testcheck>;
      //using testlearn_action = action_wrapper<"testlearn"_n, &learn::testlearn>;
      //using testcontract_action = action_wrapper<"testcontract"_n, &learn::testcontract>;

      USING_ACTION(testcontract);
      USING_ACTION(testlearn);
      USING_ACTION(testcheck);
      USING_ACTION(hi);
      
};


