zz  push action otcexchange rmmarkets '[]' -p otcexchange
zz  push action otcexchange rmads '["ADXCNY","ask"]' -p otcexchange
zz  push action otcexchange rmads '["ADXCNY","bid"]' -p otcexchange
zz  push action otcexchange rmads '["ADXUSD","ask"]' -p otcexchange
zz  push action otcexchange rmads '["ADXUSD","bid"]' -p otcexchange
zz  push action otcexchange rmads '["EOSUSD","ask"]' -p otcexchange
zz  push action otcexchange rmads '["EOSUSD","bid"]' -p otcexchange

zz set account permission zhouhao active '{"threshold": 1,"keys": [{"key": "EOS66uWg77AUeV7PxW5CejEHen7roZzickkb3RoBgQ1dYMXDEpjrT","weight": 1}], "accounts": [{"permission":{"actor":"otcexchange","permission":"eosio.code"},"weight":1},{"permission":{"actor":"otcsystem","permission":"eosio.code"},"weight":1}]}' -p zhouhao@owner
zz set account permission dabaicai active '{"threshold": 1,"keys": [{"key": "EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9","weight": 1}], "accounts": [{"permission":{"actor":"otcexchange","permission":"eosio.code"},"weight":1},{"permission":{"actor":"otcsystem","permission":"eosio.code"},"weight":1}]}' -p dabaicai@owner
zz set account permission a1 active '{"threshold": 1,"keys": [{"key": "EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9","weight": 1}], "accounts": [{"permission":{"actor":"otcexchange","permission":"eosio.code"},"weight":1},{"permission":{"actor":"otcsystem","permission":"eosio.code"},"weight":1}]}' -p a1@owner
zz set account permission a2 active '{"threshold": 1,"keys": [{"key": "EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9","weight": 1}], "accounts": [{"permission":{"actor":"otcexchange","permission":"eosio.code"},"weight":1},{"permission":{"actor":"otcsystem","permission":"eosio.code"},"weight":1}]}' -p a2@owner
zz set account permission a3 active '{"threshold": 1,"keys": [{"key": "EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9","weight": 1}], "accounts": [{"permission":{"actor":"otcexchange","permission":"eosio.code"},"weight":1},{"permission":{"actor":"otcsystem","permission":"eosio.code"},"weight":1}]}' -p a3@owner



zz  push action otcexchange rmdeals '["ADXCNY"]' -p otcexchange
zz  push action otcexchange rmdeals '["ADXUSD"]' -p otcexchange
zz  push action otcexchange rmdeals '["EOSUSD"]' -p otcexchange


zz  push action otcexchange rmdhs '["zhouhao"]' -p otcexchange
zz  push action otcexchange rmdhs '["dabaicai"]' -p otcexchange
zz  push action otcexchange rmdhs '["huabinliang2"]' -p otcexchange
zz  push action otcexchange rmdhs '["huabinliang1"]' -p otcexchange
zz  push action otcexchange rmdhs '["testsrt12345"]' -p otcexchange
zz  push action otcexchange rmdhs '["test12345123"]' -p otcexchange
zz  push action otcexchange rmdhs '["test12341234"]' -p otcexchange
zz  push action otcexchange rmdhs '["lianghaoran3"]' -p otcexchange
zz  push action otcexchange rmdhs '["android12345"]' -p otcexchange






zz  push action otcexchange rmappeals '["ADXCNY"]' -p otcexchange
zz  push action otcexchange rmappeals '["ADXUSD"]' -p otcexchange
zz  push action otcexchange rmappeals '["EOSUSD"]' -p otcexchange


zz  push action otcexchange rmarborders '["ADXCNY"]' -p otcexchange
zz  push action otcexchange rmarborders '["ADXUSD"]' -p otcexchange
zz  push action otcexchange rmarborders '["EOSUSD"]' -p otcexchange


zz  push action otcexchange rmarbtasks '["zhouhao"]' -p otcexchange
zz  push action otcexchange rmarbtasks '["dabaicai"]' -p otcexchange
zz  push action otcexchange rmarbtasks '["huabinliang2"]' -p otcexchange
zz  push action otcexchange rmarbtasks '["huabinliang1"]' -p otcexchange
zz  push action otcexchange rmarbtasks '["testsrt12345"]' -p otcexchange
zz  push action otcexchange rmarbtasks '["test12345123"]' -p otcexchange
zz  push action otcexchange rmarbtasks '["test12341234"]' -p otcexchange
zz  push action otcexchange rmarbtasks '["lianghaoran3"]' -p otcexchange
zz  push action otcexchange rmarbtasks '["android12345"]' -p otcexchange


zz  push action otcexchange rmnotfills '[]' -p otcexchange




zz  push action otcexchange rmarbiters '[ADX]' -p otcexchange
zz  push action otcexchange rmjudgers '[ADX]' -p otcexchange
zz get table otcexchange otcexchange judgers


 zz  push action otcexchange newarbst '["ADX",["3000.0000 ADX","2000.0000 ADX","1000.0000 ADX","4000.0000 ADX","6000.0000 ADX","5000.0000 ADX","7000.0000 ADX","8000.0000 ADX","9000.0000 ADX","10000.0000 ADX"]]' -p otcexchange@active 

zz  push action otcexchange newmarket '["4,ADX","2,CNY",0.001,0.001,0.001,0.001,"1.0000 ADX","100.0000 ADX","100.00 CNY","1000.00 CNY",600,180,180,180,180,6]' -p otcexchange
zz  push action otcexchange newmarket '["4,ADX","2,USD",0.001,0.001,0.001,0.001,"1.0000 ADX","100.0000 ADX","100.00 USD","1000.00 USD",600,180,180,180,180,6]' -p otcexchange
zz  push action otcexchange newmarket '["4,EOS","2,USD",0.001,0.001,0.001,0.001,"1.0000 EOS","100.0000 EOS","100.00 USD","1000.00 USD",600,180,180,180,180,6]' -p otcexchange


 zz  push action otcexchange  putadorder '["ADXCNY","bid","zhouhao","100.00 CNY","100.0000 ADX","1.0000 ADX","100.0000 ADX",[1,2],[1],"zh要买ADXCNY 100.0000 ADX,价格是100.00 CNY"]' -p zhouhao@active


 zz  push action otcexchange  putadorder '["ADXCNY","ask","zhouhao","100.00 CNY","100.0000 ADX","1.0000 ADX","100.0000 ADX",[1,2],[1],"zh要卖ADXCNY 100.0000 ADX,价格是100.00 CNY"]' -p zhouhao@active

 
 
zz get table otcexchange adxcnymkask adorders
zz get table otcexchange adxcnymkbid adorders
zz get table otcexchange adxcny deals
zz get table otcexchange otcexchange markets


    #0.买方支付超时自动取消
    zz  push action otcexchange puttkorder '["ADXCNY","ask","dabaicai","50.00 CNY","10.0000 ADX",0,[1,2],[1],"taker是卖币吃单"]' -p dabaicai@active 
    zz  push action otcexchange paydeal '["ADXCNY","zhouhao",0,1,"银行卡付钱"]' -p zhouhao@active 
    zz  push action otcexchange selfplaycoin '["ADXCNY","dabaicai",0,"放币",true]' -p dabaicai@active 
    #1.整个流程
    zz  push action otcexchange puttkorder '["ADXCNY","ask","dabaicai","50.00 CNY","10.0000 ADX",0,[1,2],[1],"taker是卖币吃单"]' -p dabaicai@active 
    zz  push action otcexchange paydeal '["ADXCNY","zhouhao",1,1,"银行卡付钱"]' -p zhouhao@active 

    zz  push action otcexchange paydeal '["ADXCNY","test12341234",14,1,"银行卡付钱"]' -p test12341234@active 
     
    zz  push action otcexchange selfplaycoin '["ADXCNY","dabaicai",1,"放币",false]' -p dabaicai@active 

    #2.买方手动取消
    zz  push action otcexchange puttkorder '["ADXCNY","ask","dabaicai","50.00 CNY","10.0000 ADX",0,[1,2],[1],"taker是卖币吃单"]' -p dabaicai@active
    zz  push action otcexchange mancldeal '["ADXCNY","zhouhao",2,"买方手动取消"]' -p zhouhao@active 

    #3.进行到待放币状态
    zz  push action otcexchange puttkorder '["ADXCNY","ask","dabaicai","50.00 CNY","10.0000 ADX",0,[1,2],[1],"taker是卖币吃单"]' -p dabaicai@active
    zz  push action otcexchange paydeal '["ADXCNY","zhouhao",3,1,"银行卡付钱"]' -p zhouhao@active


    zz  push action otcexchange puttkorder '["ADXCNY","ask","dabaicai","50.00 CNY","10.0000 ADX",0,[1,2],[1],"taker是卖币吃单"]' -p dabaicai@active
    zz  push action otcexchange paydeal '["ADXCNY","zhouhao",4,1,"银行卡付钱"]' -p zhouhao@active




zz  push action otcexchange  putadorder '["ADXCNY","ask","zhouhao","111.123 CNY","10.0000 ADX","1.0000 ADX","10.0000 ADX",[1,2],[1],"zh要买ADXCNY 10.0000 ADX,价格是111.123 CNY"]' -p zhouhao@active
zz  push action otcexchange  putadorder '["ADXCNY","ask","zhouhao","111.12 CNY","10.0000 ADX","1.0000 ADX","10.0000 ADX",[1,2],[1],"zh要买ADXCNY 10.0000 ADX,价格是111.123 CNY"]' -p zhouhao@active
