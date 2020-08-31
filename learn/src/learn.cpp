#include <learn.hpp>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>

ACTION learn::hi( name nm ) {
   /* fill in action body */
   print_f("time : %\n",111);
   print_f("Name : %\n",nm);
   print_f("account_name:%\n",_self);
  
   
}
ACTION learn::adduser(name account,uint64_t phone,std::string signture){
   require_auth(account);
   user_index users(get_self(),get_first_receiver().value);
   users.emplace(account,[&](user& user){
      user.account =account;
      user.phone = phone;
      user.regtime = time_point_sec(current_time_point());
      user.signature = signture;

   });
}

ACTION learn::edituser(name account,uint64_t phone,std::string signature){
   require_auth(account);//调用action时必须-p account@active
   user_index users(_self,_first_receiver.value);
   auto itr = users.find(account.value);//通过主键查找
   eosio::check(itr!=users.end(),"Account not found");

   users.modify(itr,account,[&](auto& user){ //写接口，需要付费
      user.phone = phone;
      user.signature = signature;

   });

}
ACTION learn::deleteuser(name account){
   require_auth(_self);//这里权限改变了
   user_index users(_self,_first_receiver.value);
   auto itr =users.require_find(account.value,"Account not found");
   auto next_itr = users.erase(itr); //返回的下一个
   print(next_itr->account," ",next_itr->phone," ",next_itr->regtime.sec_since_epoch());
}

ACTION learn::loweruser(name account){
   
  user_index users(_self, _first_receiver.value);
  auto itr = users.lower_bound(account.value);
  check(itr==users.end(),"NOT DATA");



  while(itr != users.end()){
    print(itr->account, " ",itr->phone," ",itr->primary_key());
    itr++;
  }
}

ACTION learn::addpost(name account,std::string content){
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

/*
inline Action是使用合约账户的eosio.code权限发送的，
调用者想要发送一个inline Action，需要合约账户的eosio.code权限替他做到，
即“合约名@eosio.code”权限要添加到调用者的active权限的required_auth列表中
*/

ACTION learn::sendtoken(name from, name to){
   
   require_auth(from);
  //构造一笔50个EOS的资产
  auto quantity = asset(500000, symbol("SYS", 4));
  //构造一个内联action  
  auto inline_action = action(
      permission_level{from, "active"_n},
      "eosio.token"_n,
      "transfer"_n,
      std::make_tuple(from, to, quantity, std::string(""))
    );
  //发送
  inline_action.send();
  //通知 使用require_recipient函数，可以发送一份Action的拷贝到其他账户来通知他们
  require_recipient(to);

}




/*
这个on_notify函数是写在learn合约中的，它要调用inline Action，因此需要给test账户赋予zhouhao@eosio.code权限，这里调用者和合约是同一账户，可以使用简便命令
on_notify函数能够监听eosio.token::transfer，是因为eosio.token::transfer内部调用了require_recipient(to)函数
关于on_notify
1、on_notify函数能够监听eosio.token::transfer，是因为eosio.token::transfer内部调用了require_recipient(to)函数；

2、on_notify函数不是可供直接调用的合约函数，它是一个通知处理程序（notification handler）；

3、on_notify属性是这样定义的：

[[eosio::on_notify("eosio.token::transfer")]]
必须指明监听的合约名（code）和Action名（action），这样可以避免处理从非eosio.token合约发送来的“假EOS”，提高了安全性；

4、on_notify属性接受通配符，比如这样定义：

[[eosio::on_notify("*::transfer")]]
可以让on_notify函数处理任何合约的transfer Action。
*/
[[eosio::on_notify("eosio.token::transfer")]] 
void learn::onTransfer(name from, name to, asset quantity, std::string memo){
   if(to!=_self) return;
   check(quantity.symbol==symbol("SYS",4),"must pay with EOS token");
   check(quantity.amount>10000,"must send more than 1 SYS");
   asset new_quantity = asset(quantity.amount+1,symbol("SYS",4));
   action(
      permission_level{_self,"active"_n},
      "eosio.token"_n,
      "transfer"_n,
      std::make_tuple(_self,from,new_quantity,std::string("+0.0001 SYS"))
   ).send();

}

//这个action是准备延迟执行的Action
ACTION learn::defered(name from,const std::string& msg){
   require_auth(from);
   print("Printing defered ", from,msg);

}

//send action 用于发送defered Action
ACTION learn::send(name from,const std::string& msg,uint64_t delay){
   require_auth(from);
   transaction t{};//要发送一个deferred Action，先要构造一个空的transaction，然后把deferred Action添加到transaction的actions数组中
   t.actions.emplace_back(
      permission_level{from,"active"_n},
      _self,
      "defered"_n,
      std::make_tuple(from,msg)
   );
   t.delay_sec = delay ;  //要发送一个deferred Action，先要构造一个空的transaction，然后把deferred Action添加到transaction的actions数组中
   
   
   /*调用send函数发送transaction时，需要指定一个sender_id，
   //这个id在此合约发送的所有transaction中必须是唯一的，
   //用于标识此transaction，以便进行错误处理或取消延迟交易，
   一般需要在合约中建立一个singleton数据表来记录这些id
   、延期交易的工作原理，是把需要延期执行的transaction存入区块链，因此会消耗RAM，
   transaction::send函数的第2个参数就是RAM付款账户，此交易也需要付款账户的权限签名，
   等到交易执行后会释放使用的RAM
   调用transaction::cancel_deferred(sender_id)函数，可以取消一个还未执行的延期交易
   延期交易的最大延期时间为45天，但可以通过使用新的延期交易进行覆盖，达到延长时间的目的
   如果延期交易到期时未被执行，不会收到未执行的通知
   */
   t.send(current_time_point().sec_since_epoch(),from);
   print("Send with a delay of ", delay);

}
/*
如果延期交易执行失败，EOSIO会发送一个eosio::onerror的Action，
可以使用on_notify监听此Action，从onerror对象获取sender_id和打包后的transaction数据，进行错误处理
*/
[[eosio::on_notify("eosio::onerror")]] 
void learn::onError(const onerror &error) {

   print("Resending Transaction: ", error.sender_id);
   transaction dtrx = error.unpack_sent_trx();
   dtrx.delay_sec = 3;

   dtrx.send(eosio::current_time_point().sec_since_epoch(), _self);
} 