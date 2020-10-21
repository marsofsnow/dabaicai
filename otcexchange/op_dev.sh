


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




#转账

zz get table adxio.token zhouhao account;


#adxio.token




#3.给新帐号转帐
zz push action adxio.token transfer '["adxio.token", "zhouhao","1000.0000 ADX","test"]' -p adxio.token
zz push action adxio.token transfer '["adxio.token", "dabaicai","1000.0000 ADX","test"]' -p adxio.token


zz push action eosio.token transfer '["eosio.token", "dabaicai","1000.0000 EOS","test"]' -p eosio.token


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




# 2.1 创建交易对
zz  push action otcexchange rmmarkets '[]' -p otcexchange
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

zz  push action otcexchange rmads '["ADXCNY","ask"]' -p otcexchange
zz  push action otcexchange rmads '["ADXCNY","bid"]' -p otcexchange
zz  push action otcexchange rmdeals '["ADXCNY"]' -p otcexchange
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

  zz  push action otcexchange mancldeal '["ADXCNY","zhouhao",5,"taker手动取消"]' -p zhouhao@active 

  #ACTION paydeal(const symbol_code &pair, name who, uint64_t deal_id)

  zz  push action otcexchange paydeal '["ADXCNY","zhouhao",4,1,"银行卡付钱"]' -p zhouhao@active 

  #ACTION selfplaycoin(const symbol_code &pair, name who, uint64_t deal_id, const std::string &reason, bool right_now)
   zz  push action otcexchange selfplaycoin '["ADXCNY","dabaicai",4,"放币",true]' -p dabaicai@active 

