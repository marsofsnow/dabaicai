
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
      ACTION testcheck1(){
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
      ACTION testcheck(){
         user_index users(get_self(),get_first_receiver().value);
         check(users.get_code()=="zhouhao"_n,"codes do not match!");
         check(users.get_scope()==name{"zhouhao"}.value,"scopes do not match!");
         print("match");

      }
      ACTION adduser(name account,uint64_t phone,std::string signture){
         require_auth(account);
         user_index users(get_self(),get_first_receiver().value);
         users.emplace(account,[&](user& user){
            user.account =account;
            user.phone = phone;
            user.regtime = time_point_sec(current_time_point());
            user.signature = signture;

         });
      }

      ACTION addpost(name account,std::string content){
         require_auth(account);
         user_index users(get_self(),get_first_receiver().value);

         auto itr = users.find(account.value);
         eosio::check(itr != users.end(), "account not found");

         post_index posts(get_self(), get_first_receiver().value); 
         posts.emplace(account, [&](auto& post){
         post.id = posts.available_primary_key();
         post.author = account;
         post.content = content;
      });
         

      }


      //using hi_action = action_wrapper<"hi"_n, &learn::hi>;
      //using testcheck_action = action_wrapper<"testcheck"_n, &learn::testcheck>;
      //using testlearn_action = action_wrapper<"testlearn"_n, &learn::testlearn>;
      //using testcontract_action = action_wrapper<"testcontract"_n, &learn::testcontract>;

      USING_ACTION(testcontract);
      USING_ACTION(testlearn);
      USING_ACTION(testcheck1);
      USING_ACTION(testcheck);
      USING_ACTION(addpost);
      USING_ACTION(adduser);
      USING_ACTION(hi);

   private:
      TABLE user {
         name account; //使用EOS账户名作为主键
         uint64_t phone;
         time_point_sec regtime; 
         std::string signature;
         uint64_t primary_key() const { return account.value; }
         uint64_t get_secondary_1() const { return phone; }
         uint64_t get_secondary_2() const { return regtime.sec_since_epoch(); }
      };

      TABLE post {
      uint64_t id; //使用自增id作为主键
      name author;
      std::string content;
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


