#include <learn.hpp>
#include<eosio/system.hpp>
ACTION learn::hi( name nm ) {
   /* fill in action body */
   print_f("time : %\n",111);
   print_f("Name : %\n",nm);
   print_f("account_name:%\n",_self);
  
   
}

ACTION learn::edituser(name account,uint64_t phone,std::string signature){
   require_auth(account);
   user_index users(_self,_first_receiver.value);
   auto itr = users.find(account.value);
   eosio::check(itr!=users.end(),"Account not found");

   users.modify(itr,account,[&](auto& user){
      user.phone = phone;
      user.signature = signature;

   });

}

ACTION learn::deleteuser(name account){
   require_auth(_self);//这里权限改变了
   user_index users(_self,_first_receiver.value);
   auto itr =users.require_find(account.value,"Account not found");
   auto next_itr = users.erase(itr);
   print(next_itr->account," ",next_itr->phone," ",next_itr->regtime.sec_since_epoch());
}

ACTION learn::loweruser(name account){
   
  user_index users(_self, _first_receiver.value);
  auto itr = users.lower_bound(account.value);

  while(itr != users.end()){
    print(itr->account, " ",itr->phone," ",itr->primary_key());
    itr++;
  }
}
