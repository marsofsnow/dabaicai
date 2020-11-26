


# 联调时使用的命令
# 1.创建key
zz wallet create_key -n lgz
Created new private key with a public key of: "EOS66uWg77AUeV7PxW5CejEHen7roZzickkb3RoBgQ1dYMXDEpjrT"


zz wallet create_key -n lgz
Created new private key with a public key of: "EOS7nLxFxsjRPX2x9tyCEoEcCqFntn7pcETcGpzLEEFVskGQgNei7"

zz wallet create_key -n lgz
Created new private key with a public key of: "EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9"


#2创建用户
zz system newaccount eosio --transfer zhouhao  \
EOS66uWg77AUeV7PxW5CejEHen7roZzickkb3RoBgQ1dYMXDEpjrT  \
EOS66uWg77AUeV7PxW5CejEHen7roZzickkb3RoBgQ1dYMXDEpjrT  \
--stake-net "10000.0000 EOS"  \
--stake-cpu "10000.0000 EOS"  \
--buy-ram-kbytes 8192


zz system newaccount eosio --transfer dabaicai  \
EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9  \
EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9  \
--stake-net "10000.0000 EOS"  \
--stake-cpu "10000.0000 EOS"  \
--buy-ram-kbytes 8192



zz system newaccount eosio --transfer a1  \
EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9  \
EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9  \
--stake-net "10000.0000 EOS"  \
--stake-cpu "10000.0000 EOS"  \
--buy-ram-kbytes 8192


zz system newaccount eosio --transfer a2  \
EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9  \
EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9  \
--stake-net "10000.0000 EOS"  \
--stake-cpu "10000.0000 EOS"  \
--buy-ram-kbytes 8192

zz system newaccount eosio --transfer a3  \
EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9  \
EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9  \
--stake-net "10000.0000 EOS"  \
--stake-cpu "10000.0000 EOS"  \
--buy-ram-kbytes 8192



zz system newaccount eosio --transfer a4  \
EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9  \
EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9  \
--stake-net "10000.0000 EOS"  \
--stake-cpu "10000.0000 EOS"  \
--buy-ram-kbytes 8192

zz system newaccount eosio --transfer a5  \
EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9  \
EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9  \
--stake-net "10000.0000 EOS"  \
--stake-cpu "10000.0000 EOS"  \
--buy-ram-kbytes 8192

zz system newaccount eosio --transfer a15  \
EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9  \
EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9  \
--stake-net "10000.0000 EOS"  \
--stake-cpu "10000.0000 EOS"  \
--buy-ram-kbytes 8192






#转账

zz get table adxio.token zhouhao account;


#adxio.token




#3.给新帐号转帐
zz push action adxio.token transfer '["adxio.token", "zhouhao","100000.0000 ADX","test"]' -p adxio.token
zz push action adxio.token transfer '["adxio.token", "dabaicai","100000.0000 ADX","test"]' -p adxio.token


zz push action adxio.token transfer '["adxio.token", "a1","10000.0000 ADX","test"]' -p adxio.token
zz push action adxio.token transfer '["adxio.token", "a2","10000.0000 ADX","test"]' -p adxio.token
zz push action adxio.token transfer '["adxio.token", "a3","10000.0000 ADX","test"]' -p adxio.token
zz push action adxio.token transfer '["adxio.token", "a4","10000.0000 ADX","test"]' -p adxio.token
zz push action adxio.token transfer '["adxio.token", "a5","10000.0000 ADX","test"]' -p adxio.token
zz push action adxio.token transfer '["adxio.token", "a15","10000.0000 ADX","test"]' -p adxio.token





zz push action adxio.token transfer '["adxio.token", "android12345","10000.0000 ADX","test"]' -p adxio.token
zz push action adxio.token transfer '["adxio.token", "test12345123","10000.0000 ADX","test"]' -p adxio.token


zz push action adxio.token transfer '["adxio.token", "bbbbbb111111","100000.0000 ADX","test"]' -p adxio.token
zz push action adxio.token transfer '["adxio.token", "bbbbbb111112","100000.0000 ADX","test"]' -p adxio.token
zz push action adxio.token transfer '["adxio.token", "bbbbbb111113","100000.0000 ADX","test"]' -p adxio.token
zz push action adxio.token transfer '["adxio.token", "bbbbbb111114","100000.0000 ADX","test"]' -p adxio.token
zz push action adxio.token transfer '["adxio.token", "bbbbbb111115","100000.0000 ADX","test"]' -p adxio.token
zz push action adxio.token transfer '["adxio.token", "ssssss111111","100000.0000 EOS","test"]' -p adxio.token
zz push action adxio.token transfer '["adxio.token", "otcexchange","100000.0000 EOS","test"]' -p adxio.token

zz push action eosio.token transfer '["eosio.token", "dabaicai","10000.0000 EOS","test"]' -p eosio.token
zz push action eosio.token transfer '["eosio.token", "zhouhao","10000.0000 EOS","test"]' -p eosio.token
zz push action eosio.token transfer '["eosio.token", "otcexchange","10000.0000 EOS","test"]' -p eosio.token
zz push action eosio.token transfer '["eosio.token", "ssssss111111","10000.0000 EOS","test"]' -p eosio.token
zz push action eosio.token transfer '["eosio.token", "ssssss111111","10000.0000 EOS","test"]' -p eosio.token


zz get table adxio.token dabaicai accounts

#4.抵押cpu 内存 买RAM

zz  system buyram otcexchange   otcexchange "1000 EOS"   # 自己给自己买RAM
zz  system buyram dabaicai   dabaicai "1000.0000 EOS"   # 自己给自己买RAM
zz  system buyram eosio  otc "1000 EOS"   # 别人给maker买RAM

zz system delegatebw maker otc '500.0000 EOS' '500.0000 EOS' -p otc # 自己抵押
zz system delegatebw maker otc '500.0000 EOS' '500.0000 EOS' -p otc # 别人给自己抵押





#查询
zz get account  maker
zz get currency balance eosio.token otcexchange
zz get table eosio eosio voters 


#升级
zz  push action otcexchange rmmarkets '[]' -p otcexchange
zz  push action otcexchange rmads '["ADXCNY","ask"]' -p otcexchange
zz  push action otcexchange rmads '["ADXCNY","bid"]' -p otcexchange
zz  push action otcexchange rmdeals '["ADXCNY"]' -p otcexchange

zz  push action otcexchange rmappeals '["ADXCNY"]' -p otcexchange



# 2.1 创建交易对

zz  push action otcexchange rmads '[]' -p otcexchange
zz  push action otcexchange rmdeals '[]' -p otcexchange

zz  push action otcexchange newmarket '["4,ADX","2,CNY","0.1000 ADX","0.1000 ADX","0.1000 ADX","0.1000 ADX","1.0000 ADX","100.0000 ADX","100.00 CNY","1000.00 CNY",60,180,180,180,180,6]' -p otcexchange
zz  push action otcexchange newmarket '["4,ADX","2,USD","0.1000 ADX","0.1000 ADX","0.1000 ADX","0.1000 ADX","1.0000 ADX","100.0000 ADX","100.00 USD","1000.00 USD",60,180,180,180,180,6]' -p otcexchange
zz  push action otcexchange newmarket '["4,EOS","2,USD","0.1000 EOS","0.1000 EOS","0.1000 EOS","0.1000 EOS","1.0000 EOS","100.0000 EOS","100.00 USD","1000.00 USD",60,180,180,180,180,6]' -p otcexchange



zz  get table otcexchange otcexchange markets
zz  push action otcexchange closemarket '["ADXCNY"]' -p otcexchange
zz  push action otcexchange openmarket '["ADXCNY"]' -p otcexchange
zz  push action otcexchange rmmarket  '["ADXCNY"]' -p otcexchange


zz  get table  otcexchange otcexchange markets --key-type i64 --index 3 -L "4,ADX" -U "4,ADX";
zz  get table  otcexchange adxcny deals --key-type i64 --index 2 -L "huabinliang1" -U "huabinliang1";

zz  get table  otcexchange otcexchange markets --key-type i64 --index 2  -U 1






   ACTION putadorder(const symbol_code &pair,
                     const std::string &side,
                     name user,
                     asset price,
                     asset amount,
                     asset amount_min,
                     asset amount_max,
                     const std::string &source)

# 

# 挂一个买单

zz get table otcexchange ADXCNY adorders

zz  push action otcexchange  putadorder '["ADXCNY","bid","zhouhao","100.00 CNY","10.0000 ADX","1.0000 ADX","1.0000 ADX","zh要买ADXCNY 10.0000 ADX,价格是10.00 CNY"]' -p zhouhao@active
 
#挂一个卖单

zz  push action otcexchange  putadorder '["ADXCNY","ask","zhouhao","100.00 CNY","10.0000 ADX","1.0000 ADX","1.0000 ADX","zh要卖ADXCNY 10.0000 ADX,价格是100.00 CNY"]' -p zhouhao

#合约内联调用的权限
zz  set account permission otcexchange  active --add-code 


zz get table otcexchange adxcnymkask adorders
zz get table otcexchange adxcnymkbid adorders
zz get table otcexchange adxcny deals




# 登录
zz push push action otcsystem login '["zhouhao","2222","abcd","abcd"]' -p zhouhao@active
zz push push action otcsystem login '["dabaicai","2222","ab","ad"]' -p dabaicai@active


zz push push action otcexchange offad '["ADXCNY","ask",0,"test"]' -p zhouhao
zz push push action otcexchange offad '["ADXCNY","bid",1,"test"]' -p zhouhao



zz set account permission zhouhao active '{"threshold": 1,"keys": [{"key": "EOS66uWg77AUeV7PxW5CejEHen7roZzickkb3RoBgQ1dYMXDEpjrT","weight": 1}], "accounts": [{"permission":{"actor":"otcexchange","permission":"eosio.code"},"weight":1}]}' -p zhouhao@owner



#用户授权
zz set account permission zhouhao active '{"threshold": 1,"keys": [{"key": "EOS66uWg77AUeV7PxW5CejEHen7roZzickkb3RoBgQ1dYMXDEpjrT","weight": 1}], "accounts": [{"permission":{"actor":"otcexchange","permission":"eosio.code"},"weight":1},{"permission":{"actor":"otcsystem","permission":"eosio.code"},"weight":1}]}' -p zhouhao@owner
zz set account permission dabaicai active '{"threshold": 1,"keys": [{"key": "EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9","weight": 1}], "accounts": [{"permission":{"actor":"otcexchange","permission":"eosio.code"},"weight":1},{"permission":{"actor":"otcsystem","permission":"eosio.code"},"weight":1}]}' -p dabaicai@owner


#



#手动下架一个广告
zz push push action otcexchange offad '["ADXCNY","ask","0","手动下架"]' -p zhouhao@active
zz push push action otcexchange offad '["ADXCNY","ask","1","手动下架"]' -p zhouhao@active
zz push push action otcexchange offad '["ADXCNY","bid",0,"test"]' -p zhouhao
zz get table adxio.token zhouhao accounts
zz get table adxio.token dabaicai accounts

#来一个卖币taker

 zz  push action otcexchange puttkorder '["ADXCNY","ask","dabaicai","50.00 CNY","1.0000 ADX",1,"taker是卖币吃单"]' -p dabaicai 

 zz push action otcexchange rmmarkets '[]' -p otcexchange




#发1个广告买单，买50个ADX，价格是100.00CNY ，交易数量[1,50]
 zz  push action otcexchange  putadorder '["ADXCNY","bid","zhouhao","100.00 CNY","50.0000 ADX","1.0000 ADX","50.0000 ADX","zh要买ADXCNY 50.0000 ADX,价格是100.00 CNY"]' -p zhouhao@active

 #发一个卖币吃单，
 zz  push action otcexchange puttkorder '["ADXCNY","ask","dabaicai","50.00 CNY","10.0000 ADX",1,"taker是卖币吃单"]' -p dabaicai@active 
 zz get table otcexchange adxcny deals

 #ACTION mancldeal(const symbol_code &pair, name who, uint64_t deal_id, const std::string &reason);

  zz  push action otcexchange mancldeal '["ADXCNY","zhouhao",6,"买方手动取消"]' -p zhouhao@active 

  #ACTION paydeal(const symbol_code &pair, name who, uint64_t deal_id)

  zz  push action otcexchange paydeal '["ADXCNY","zhouhao",4,1,"银行卡付钱"]' -p zhouhao@active 

  #ACTION selfplaycoin(const symbol_code &pair, name who, uint64_t deal_id, const std::string &reason, bool right_now)
   zz  push action otcexchange selfplaycoin '["ADXCNY","dabaicai",4,"放币",true]' -p dabaicai@active 


    #买方支付超时自动取消
    zz  push action otcexchange puttkorder '["ADXCNY","ask","dabaicai","50.00 CNY","10.0000 ADX",0,"taker是卖币吃单"]' -p dabaicai@active 
    
    #整个流程
    zz  push action otcexchange puttkorder '["ADXCNY","ask","dabaicai","50.00 CNY","10.0000 ADX",0,"taker是卖币吃单"]' -p dabaicai@active 
    zz  push action otcexchange paydeal '["ADXCNY","zhouhao",1,1,"银行卡付钱"]' -p zhouhao@active 
    zz  push action otcexchange selfplaycoin '["ADXCNY","dabaicai",1,"放币",false]' -p dabaicai@active 

    #买方手动取消
    zz  push action otcexchange puttkorder '["ADXCNY","ask","dabaicai","50.00 CNY","10.0000 ADX",0,"taker是卖币吃单"]' -p dabaicai@active
    zz  push action otcexchange mancldeal '["ADXCNY","zhouhao",2,"买方手动取消"]' -p zhouhao@active 


#
 zz  push action otcexchange  putadorder '["ADXCNY","bid","zhouhao","100.00 CNY","100.0000 ADX","1.0000 ADX","100.0000 ADX","zh要买ADXCNY 100.0000 ADX,价格是100.00 CNY"]' -p zhouhao@active
  #ACTION defcldeal(const symbol_code &pair, name who, uint64_t deal_id, uint8_t status, const std::string &reason);

  zz  push action otcexchange defcldeal '["ADXCNY","zhouhao",7,62,"taker是卖币吃单，测试"]' -p zhouhao@active 





  zz  push action otcexchange newarbst '["ADX",["3000.0000 ADX","2000.0000 ADX","1000.0000 ADX","4000.0000 ADX","6000.0000 ADX","5000.0000 ADX","7000.0000 ADX","8000.0000 ADX","9000.0000 ADX","10000.0000 ADX"]]' -p otcexchange@active 
  zz  push action otcexchange rmarbst '["ADX"]' -p otcexchange@active 
  zz  push action otcexchange rmarbsts '[]' -p otcexchange@active 

  zz get table otcexchange otcexchange arbsts

  #注册成为仲裁员
#ACTION otcexchange::regarbiter(name arbitername, const symbol_code &stock, asset mortgage, uint32_t online_begin, uint32_t online_end)
  zz  push action otcexchange regarbiter '["a1","ADX","1000.0000 ADX",60,82800,"656717520@qq.com","1234"]' -p a1@active 
  zz  push action otcexchange regarbiter '["a2","ADX","2000.0000 ADX",120,82800,"656717520@qq.com","1234"]' -p a2@active 
  zz  push action otcexchange regarbiter '["a3","ADX","3000.0000 ADX",180,82800,"656717520@qq.com","1234"]' -p a3@active 
  zz  push action otcexchange regarbiter '["a4","ADX","3000.0000 ADX",180,82800,"656717520@qq.com","1234"]' -p a4@active 
  zz  push action otcexchange regarbiter '["a5","ADX","3000.0000 ADX",180,82800,"656717520@qq.com","1234"]' -p a5@active 
  zz  push action otcexchange regarbiter '["a16","ADX","3000.0000 ADX",180,82800,"656717520@qq.com","1234"]' -p a16@active 


  zz  push action otcexchange regarbiter '["bbbbbb111111","ADX","1000.0000 ADX",180,82800,"656717520@qq.com","1234"]' -p bbbbbb111111@active 
  zz  push action otcexchange regarbiter '["bbbbbb111112","ADX","2000.0000 ADX",280,82800,"656717520@qq.com","1234"]' -p bbbbbb111112@active 
  zz  push action otcexchange regarbiter '["bbbbbb111113","ADX","3000.0000 ADX",380,82800,"656717520@qq.com","1234"]' -p bbbbbb111113@active 
  zz  push action otcexchange regarbiter '["bbbbbb111114","ADX","4000.0000 ADX",480,82800,"656717520@qq.com","1234"]' -p bbbbbb111114@active 
  zz  push action otcexchange regarbiter '["bbbbbb111115","ADX","5000.0000 ADX",580,82800,"656717520@qq.com","1234"]' -p bbbbbb111115@active 
  zz  push action otcexchange unregarbiter '["a1","ADX","XXXX"]' -p a1@active 
  zz get table otcexchange adx  arbiters
  zz get table otcexchange a2  arbers
  zz get table otcexchange a3  arbers
  zz push action otcexchange rmarbiter '["a1]' -p otcexchange

   zz get table otcexchange adx  arbitrators
   zz get table otcexchange adx  judgers

   zz get table otcexchange a1 userroles --key-type i64 --index 1 -L "ADX" -U "ADX";
 
  #ACTION otcexchange::unregarbiter(name arbitername, const symbol_code &stock, const std::string &reason)

zz  push action otcexchange regjuder '["otcexchange","ADX",0,86400,"656717520@qq.com","1234"]' -p otcexchange@active 
zz  push action otcexchange moduserrole '["lianghaoran4","ADX",2]' -p lianghaoran4@active 
zz  push action otcexchange moduserrole '["lianghaoran3","ADX",2]' -p lianghaoran3@active 
zz  push action otcexchange moduserrole '["huabinliang2","ADX",2]' -p huabinliang2@active 
zz  push action otcexchange moduserrole '["huabinliang3","ADX",2]' -p huabinliang3@active 

ACTION putappeal(name who,
                    const std::string &side,
                    const symbol_code &pair,
                    uint64_t deal_id,
                    const std::map<std::string, std::string> &attachments,
                    const std::vector<std::string> &vec_img,
                    const std::vector<std::string> &vec_video,
                    const std::string &source);

 zz  push action otcexchange puttkorder '["ADXCNY","ask","dabaicai","50.00 CNY","10.0000 ADX",1,[1,2],[1],"仲裁1"]' -p dabaicai@active 
 zz  push action otcexchange paydeal '["ADXCNY","zhouhao",7,1,"银行卡付钱"]' -p zhouhao@active 

 zz get table otcexchange  adxcny appeals





#deal id =2 dabaicai side:1  phone email reason desc



  zz  push action otcexchange  putappeal '["zhouhao","bid","ADXCNY",7,"18588250437","656717520@qq.com","原因1","描述1",["http://zhongyingying.qicp.io:28080/ipfs/QmcnSL6oApFVYDb4wmUD5XLxxBzFW2H51f2Sdu3QjcFZqA"],["url"],"买家发起申诉"]' -p zhouhao

  zz get table otcexchange  adxcny arborders
  zz get table otcexchange  adxcny appeals
  zz get table otcexchange  adxcny deals
   zz get table otcexchange  a3 arbtasks
   zz get table otcexchange otcexchange markets

  
#ACTION otcexchange::arbdeal(name arbiter, const symbol_code &pair, uint64_t deal_id, uint8_t choice, const std::string &reason)


 zz  push action otcexchange  arbdeal '["a1","ADXCNY",7,1,"买家已付款，应该放币"]' -p a1
 zz  push action otcexchange  arbdeal '["a2","ADXCNY",7,1,"买家已付款，应该放币"]' -p a2
 zz  push action otcexchange  arbdeal '["a3","ADXCNY",7,1,"买家已付款，应该放币"]' -p a3
 zz  push action otcexchange  arbdeal '["bbbbbb111115","EOSCNY",2,1,"应该放币"]' -p bbbbbb111115
 zz  push action otcexchange putjudge '["dabaicai","ADXCNY",7,"卖家上诉"]' -p dabaicai
 
    ACTION putjudge(name failer,
                   const symbol_code &pair,
                   uint64_t deal_id,
                   const std::string &reason);
ACTION judgedeal(name judger, const symbol_code &pair, uint64_t deal_id, uint8_t choice, const std::string &reason);

 zz  push action otcexchange judgedeal '["otcexchange","ADXCNY",7,2,"终审卖方不放币，是恶意仲裁"]' -p otcexchange



zz  get table  otcexchange adxcny deals --key-type i64 --index 1 -L 4 -U 4;


zz  get table  otcexchange a1 arbtasks --key-type i64 --index 2 -L 1 -U 1;
zz  get table  otcexchange a2 arbtasks --key-type i64 --index 2 -L 1 -U 1;
zz  get table  otcexchange a3 arbtasks --key-type i64 --index 2 -L 1 -U 1;
zz  get table  otcexchange a2 arbtasks --key-type i64 --index 2  -U 1

  
  zz  push action otcexchange modfeerate '["zhouhao","4,ADX","2.0000 ADX"]' -p zhouhao
  
  zz  push action otcexchange rmnotfills '[]' -p otcexchange

  zz  get table  otcexchange adxcny xappeals --key-type i64 --index 1 -L 4 -U 4;


  zz get table otcexchange  adxcny arborders --key-type i64 --index 1 -L 4 -U 4;
  zz  get table  otcexchange adxcny deals --key-type i64 --index 1 -L 13 -U 13;


  zz  get table  otcexchange adxcny judges --key-type i64 --index 1 -L 3 -U 3


ACTION otcexchange::putappeal(name who,
                              const std::string &side,
                              const symbol_code &pair,
                              uint64_t deal_id,
                              const std::vector<std::string> &vec_contacts,
                              const std::string &reason,
                              const std::string &desc,
                              const std::vector<std::string> &vec_img,
                              const std::vector<std::string> &vec_video,
                              const std::string &source)


  zz  push action otcexchange  putappeal '["dabaicai","ask","ADXCNY",4,["18588250437","656717520@qq.com"],"原因1","描述1",["http://zhongyingying.qicp.io:28080/ipfs/QmcnSL6oApFVYDb4wmUD5XLxxBzFW2H51f2Sdu3QjcFZqA"],["url"],"卖家发起申诉"]' -p dabaicai




  zz get table otcexchange zhouhao balancelogs
  zz get table otcexchange dabaicai balancelogs

  zz get table otcexchange   adx  arbitrators  --key-type i64 --index 2 -L 2


//"EOS8GnxsyzChJF4pobmcvYan3qv1ovw66ypYNiJZatSWKkejfEqg4",
//"5K8bmn8AMNewSzgB3VNnz7pahVVLTF7LaksnF8tjoSPVcvS2xDw"

cleos wallet private_keys -n lgz
密码 PW5KCNg2etNZABk9MDcbHLCCjDGYTYiSqZN78wtY7rTx5b7j2c7yT

[root@localhost wallets]#  cleos wallet private_keys -n default
PW5JL3SP3tG42sch9z5Li9eaYMM96QXA5z6b23mqd5nBpdMKstsAj





    #3.进行到待放币状态
    zz  push action otcexchange puttkorder '["ADXCNY","ask","dabaicai","50.00 CNY","10.0000 ADX",0,[1,2],[1],"taker是卖币吃单"]' -p dabaicai@active
    zz  push action otcexchange paydeal '["ADXCNY","zhouhao",6,1,"银行卡付钱"]' -p zhouhao@active

    #买家发起申诉zhouhao，买家已经付款，
    zz  push action otcexchange  putappeal '["zhouhao","bid","ADXCNY",6,["18588250437","656717520@qq.com"],"卖家不放币","买家已经支付",["http://zhongyingying.qicp.io:28080/ipfs/QmcnSL6oApFVYDb4wmUD5XLxxBzFW2H51f2Sdu3QjcFZqA"],["url"],"买家发起申诉"]' -p zhouhao
    
    #a1 a2 a3 都仲裁放币

    zz  push action otcexchange  arbdeal '["a1","ADXCNY",6,2,"买家已付款，应该放币"]' -p a1
    zz  push action otcexchange  arbdeal '["a2","ADXCNY",6,2,"买家已付款，应该放币"]' -p a2
    zz  push action otcexchange  arbdeal '["a3","ADXCNY",6,2,"买家已付款，应该放币"]' -p a3

    #失败方发起终审
    zz  push action otcexchange  putjudge '["dabaicai","ADXCNY",3,["18588250437","656717520@qq.com"],"买家其实没有付款","买家作弊",["http://zhongyingying.qicp.io:28080/ipfs/QmcnSL6oApFVYDb4wmUD5XLxxBzFW2H51f2Sdu3QjcFZqA"],["url"],"卖家发起终审"]' -p dabaicai
    #终审员终审
    zz  push action otcexchange judgedeal '["otcexchange","ADXCNY",3,2,"终审卖方不放币，是恶意仲裁"]' -p otcexchange
    zz  push action otcexchange moddealss '["ADXCNY",4,31"人工修正状态"]' -p otcexchange



    "source": "huabinliang2 bid 吃单:wait pay|买方huabinliang2已支付,payed,wait playcoin|huabinliang2发起申诉:bider appeal|第一个仲裁员bbbbbb111112仲裁:arbiarating|共识取消放币:bbbbbb111114仲裁:arbiarate cancel playcoin|arb deal cancel"