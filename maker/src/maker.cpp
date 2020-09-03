#include <maker.hpp>
#include <eosio/system.hpp>
#include <eosio/crypto.hpp>
#include <map>
#include <cstring>
#include <ctime>
//#include <boost/timer.hpp>


ACTION maker::hi( name nm ) {
   //boost::timer t;
   /* fill in action body */
    //输出错误信息
    uint64_t a=1;
    uint64_t b=3;
   //check(a == b, "a does not equal b");
   //输出错误码
   //check(a == b, 1);

   std::string str="周浩";
   print_f("测试std::string的输出:%\n",str);

   print_f("================测试std::map<int,std::string>的输出:==============\n");
   std::map<int,std::string> m{
      {1,"1"},
      {2,"2"}
   };
   for(auto& item:m){
      print_f("key=%,value=%\n",item.first,item.second);
   }
   print_f("================测试std::map<int,std::string>的输出:=============\n");
   sha256("songguo", std::strlen("songguo")).print();  
   print("\n");
   //eosio_exit(1);

   print_f("Name : % %\n",nm,str);


    
   //print_f("action elapsed:%s\n",t.elapsed());

   print_f("get_self()=%\n",get_self());            
   print_f("get_first_receiver()=%\n",get_first_receiver());        
   

}

ACTION maker::wrapperhi(name nm){
   hi_action act("maker"_n,permission_level{nm,"active"_n});
   print("use action_wrapper begin.....");
   act.send(nm);
   print("use action_wrapper end.....");
   
}

ACTION maker::insert(name account,std::string tip){
   require_auth(account); //调用action时，必须-p account@active
   order_index_t orders(_self,_first_receiver.value);
   orders.emplace(account,[&](order_t& order){
      order.id      = orders.available_primary_key();
      order.account = account;
      order.ctime   = time_point_sec(current_time_point());
      order.utime   = order.utime;
      order.tip     = tip;

   });
}

ACTION maker::erase(){

}
ACTION maker::update(){

}
ACTION maker::query(){

}

ACTION maker::getbyaccount(name account){
   require_auth(account);
   order_index_t orders(_self,_first_receiver.value);
   auto account_index = orders.get_index<"byaccount"_n>();
   auto itr = account_index.find(account.value);
   while (itr!=account_index.end())
   {
      print_f("id=% ,account=%, ctime=%,utime=%,tip=%\n",
              itr->id,
              itr->account.to_string(),
              /*itr->ctime.sec_since_epoch(),
              itr->utime.sec_since_epoch(),*/
              itr->tip
             );
      itr++;
      
   }
   

}

ACTION maker::defered(  name from, const std::string& msg){
   require_auth(from);
   print_f("延迟的 %f 的消息:% 开始执行",from,msg);

}




ACTION maker::sendtoken(name from, name to){
   require_auth(from);
   asset quanlity = asset(500000,symbol("SYS",4));
   std::string msg(from.to_string()+"转账给"+to.to_string()+quanlity.to_string());
   action inline_action = action(
      permission_level{from,"active"_n}, //需要转账账号的active权限
      "eosio.token"_n,
      "transfer"_n,
      std::make_tuple(from,to,quanlity,msg)
   );
   //发送
   inline_action.send();
   //通知
   require_recipient(to); //告知to sendtoken被调用

   /*
      3、要发送一个deferred Action，先要构造一个空的transaction，然后把deferred Action添加到transaction的actions数组中；

      4、通过给此transaction设置delay_sec（延迟秒数），达到延迟执行此transaction的目的（deferred transaction）；

      5、调用send函数发送transaction时，需要指定一个sender_id，这个id在此合约发送的所有transaction中必须是唯一的，用于标识此transaction，以便进行错误处理或取消延迟交易，一般需要在合约中建立一个singleton数据表来记录这些id；

      6、延期交易的工作原理，是把需要延期执行的transaction存入区块链，因此会消耗RAM，transaction::send函数的第2个参数就是RAM付款账户，此交易也需要付款账户的权限签名，等到交易执行后会释放使用的RAM；

      7、调用transaction::cancel_deferred(sender_id)函数，可以取消一个还未执行的延期交易；

      8、如果延期交易执行失败，EOSIO会发送一个eosio::onerror的Action，可以使用on_notify监听此Action，从onerror对象获取sender_id和打包后的transaction数据，进行错误处理；

      9、延期交易的最大延期时间为45天，但可以通过使用新的延期交易进行覆盖，达到延长时间的目的；

      10、如果延期交易到期时未被执行，不会收到未执行的通知。
      */

      transaction t{};
      t.actions.emplace_back(
         permission_level(from,"active"_n),
         _self,
         "defered"_n,
         std::make_tuple(from,msg)
      );
      t.delay_sec = 60*3;
      t.send(current_time_point().sec_since_epoch(),from);
      print_f("Sent with a delay of % ",t.delay_sec.value);

}

//必须指明监听的合约名（code）和Action名（action）
//[[eosio::on_notify("*::transfer")]] //n_notify函数处理任何合约的transfer Action
[[eosio::on_notify("eosio.token::transfer")]] 
void maker::on_transfer(name from,name to,asset quantity,std::string msg){ //on_transfer的参数完全是action的数据
   if(to!=_self) return;
   eosio::check(quantity.symbol == symbol("SYS", 4), "must pay with SYS token");  
   eosio::check(quantity.amount > 10000, "must send more than 1 SYS");
   print_f("% 收到 % 的转账:%,%",to,from,quantity.to_string(),msg);
}

[[eosio::on_notify("eosio::onerror")]] 
void maker::onError(const onerror &error){
   print("Resending Transaction: ", error.sender_id);
   transaction dtrx = error.unpack_sent_trx();
   dtrx.delay_sec = 3;
   dtrx.send(eosio::current_time_point().sec_since_epoch(), _self);
}



