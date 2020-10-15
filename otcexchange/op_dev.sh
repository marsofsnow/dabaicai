


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


zz get table adxio.token dabaicai accounts

#4.抵押cpu 内存 买RAM

zz  system buyram otcexchange   otcexchange "1000 EOS"   # 自己给自己买RAM
zz  system buyram eosio  otc "1000 EOS"   # 别人给maker买RAM

zz system delegatebw maker otc '500.0000 EOS' '500.0000 EOS' -p otc # 自己抵押
zz system delegatebw maker otc '500.0000 EOS' '500.0000 EOS' -p otc # 别人给自己抵押




#查询
zz get account  maker
zz get currency balance eosio.token otcexchange
zz get table eosio eosio voters 




# 2.1 创建交易对
zz  push action otcexchange newmarket '["4,ADX","2,CNY","0.1000 ADX","0.1000 ADX","1.0000 ADX","100.0000 ADX","100.00 CNY","1000.00 CNY",60,180,180,180,180,6]' -p otcexchange

zz  push action otcexchange newmarket '["4,ADX","2,USD","0.1000 ADX","0.1000 ADX","1.0000 ADX","100.0000 ADX","100.00 USD","1000.00 USD",60,180,180,180,180,6]' -p otcexchange
zz  get table otcexchange otcexchange markets
zz  push action otcexchange closemarket '["ADXCNY"]' -p otcexchange
zz  push action otcexchange openmarket '["ADXCNY"]' -p otcexchange
#zz  push action otcexchange rmmarket  '["ADXCNY"]' -p otcexchange
#zz  push action otcexchange rmmarkets '[]' -p otcexchange
zz  get table  otcexchange otcexchange markets --key-type i64 --index 1  -L ADXCNY -U ADXCNY
zz  get table  otcexchange otcexchange markets --key-type i64 --index 2  -U 1


   ACTION putadorder(const symbol_code &pair,
                     const std::string &side,
                     name user,
                     asset price,
                     asset amount,
                     asset amount_min,
                     asset amount_max,
                     const std::string &source)
# 挂一个买单

zz  push action otcexchange  putadorder '["ADXCNY","bid","zhouhao","100.00 CNY","10.0000 ADX","1.0000 ADX","1.0000 ADX","周浩要买ADXCNY 10.0000 ADX,价格是10.00 CNY"]' -p zhouhao@active
 
#挂一个卖单

zz  push action otcexchange  putadorder '["ADXCNY","ask","zhouhao","100.00 CNY","10.0000 ADX","1.0000 ADX","1.0000 ADX","周浩要买ADXCNY 10.0000 ADX,价格是100.00 CNY"]' -p zhouhao

#内联调用的权限
zz  set account permission otcexchange  active --add-code 


zz set account permission zhouhao active '{"threshold": 1,"keys": [{"key": "EOS66uWg77AUeV7PxW5CejEHen7roZzickkb3RoBgQ1dYMXDEpjrT","weight": 1}], "accounts": [{"permission":{"actor":"test","permission":"eosio.code"},"weight":1}]}' -p zhouhao@owner