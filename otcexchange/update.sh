zz  push action otcexchange rmmarkets '[]' -p otcexchange
zz  push action otcexchange rmads '["ADXCNY","ask"]' -p otcexchange
zz  push action otcexchange rmads '["ADXCNY","bid"]' -p otcexchange
zz  push action otcexchange rmdeals '["ADXCNY"]' -p otcexchange


zz  push action otcexchange newmarket '["4,ADX","2,CNY","0.1000 ADX","0.1000 ADX","0.1000 ADX","0.1000 ADX","1.0000 ADX","100.0000 ADX","100.00 CNY","1000.00 CNY",60,180,180,180,180,6]' -p otcexchange
zz  push action otcexchange newmarket '["4,ADX","2,USD","0.1000 ADX","0.1000 ADX","0.1000 ADX","0.1000 ADX","1.0000 ADX","100.0000 ADX","100.00 USD","1000.00 USD",60,180,180,180,180,6]' -p otcexchange
zz  push action otcexchange newmarket '["4,EOS","2,USD","0.1000 EOS","0.1000 EOS","0.1000 EOS","0.1000 EOS","1.0000 EOS","100.0000 EOS","100.00 USD","1000.00 USD",60,180,180,180,180,6]' -p otcexchange


 zz  push action otcexchange  putadorder '["ADXCNY","bid","zhouhao","100.00 CNY","100.0000 ADX","1.0000 ADX","100.0000 ADX","zh要买ADXCNY 100.0000 ADX,价格是100.00 CNY"]' -p zhouhao@active
 zz  push action otcexchange  putadorder '["ADXCNY","bid","zhouhao","100.00 CNY","100.0000 ADX","1.0000 ADX","100.0000 ADX","zh要买ADXCNY 100.0000 ADX,价格是100.00 CNY"]' -p zhouhao@active
 zz  push action otcexchange  putadorder '["ADXCNY","bid","zhouhao","100.00 CNY","100.0000 ADX","1.0000 ADX","100.0000 ADX","zh要买ADXCNY 100.0000 ADX,价格是100.00 CNY"]' -p zhouhao@active
 zz  push action otcexchange  putadorder '["ADXCNY","bid","zhouhao","100.00 CNY","100.0000 ADX","1.0000 ADX","100.0000 ADX","zh要买ADXCNY 100.0000 ADX,价格是100.00 CNY"]' -p zhouhao@active


 zz  push action otcexchange  putadorder '["ADXCNY","ask","zhouhao","100.00 CNY","100.0000 ADX","1.0000 ADX","100.0000 ADX","zh要卖ADXCNY 100.0000 ADX,价格是100.00 CNY"]' -p zhouhao@active
 zz  push action otcexchange  putadorder '["ADXCNY","ask","zhouhao","100.00 CNY","100.0000 ADX","1.0000 ADX","100.0000 ADX","zh要卖ADXCNY 100.0000 ADX,价格是100.00 CNY"]' -p zhouhao@active
 zz  push action otcexchange  putadorder '["ADXCNY","ask","zhouhao","100.00 CNY","100.0000 ADX","1.0000 ADX","100.0000 ADX","zh要卖ADXCNY 100.0000 ADX,价格是100.00 CNY"]' -p zhouhao@active
 zz  push action otcexchange  putadorder '["ADXCNY","ask","zhouhao","100.00 CNY","100.0000 ADX","1.0000 ADX","100.0000 ADX","zh要卖ADXCNY 100.0000 ADX,价格是100.00 CNY"]' -p zhouhao@active

 
 
 
zz get table otcexchange adxcnymkask adorders
zz get table otcexchange adxcnymkbid adorders
zz get table otcexchange adxcny deals


    #0.买方支付超时自动取消
    zz  push action otcexchange puttkorder '["ADXCNY","ask","dabaicai","50.00 CNY","10.0000 ADX",0,"taker是卖币吃单"]' -p dabaicai@active 
    
    #1.整个流程
    zz  push action otcexchange puttkorder '["ADXCNY","ask","dabaicai","50.00 CNY","10.0000 ADX",0,"taker是卖币吃单"]' -p dabaicai@active 
    zz  push action otcexchange paydeal '["ADXCNY","zhouhao",1,1,"银行卡付钱"]' -p zhouhao@active 
    zz  push action otcexchange selfplaycoin '["ADXCNY","dabaicai",1,"放币",false]' -p dabaicai@active 

    #2.买方手动取消
    zz  push action otcexchange puttkorder '["ADXCNY","ask","dabaicai","50.00 CNY","10.0000 ADX",0,"taker是卖币吃单"]' -p dabaicai@active
    zz  push action otcexchange mancldeal '["ADXCNY","zhouhao",2,"买方手动取消"]' -p zhouhao@active 

    #3.进行到待放币状态
    zz  push action otcexchange puttkorder '["ADXCNY","ask","dabaicai","50.00 CNY","10.0000 ADX",0,"taker是卖币吃单"]' -p dabaicai@active
    zz  push action otcexchange paydeal '["ADXCNY","zhouhao",3,1,"银行卡付钱"]' -p zhouhao@active


