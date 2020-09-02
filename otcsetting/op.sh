#1.给帐号创建密钥
cc wallet create_key -n hopex

#2.创建帐号
cc system newaccount eosio otcsetting  \
EOS7Jf8FP6tibGuwF6DuY7SqJQUPMYLmmHNQLBLwagJybM6K9ALco \
EOS7Jf8FP6tibGuwF6DuY7SqJQUPMYLmmHNQLBLwagJybM6K9ALco  \
--stake-net '50.00 SYS' \
--stake-cpu '50.00 SYS' \
--buy-ram-kbytes 10000


#3.给新帐号转帐
cc push action eosio.token transfer '["eosio", "maker","1.0000 SYS","test"]' -p eosio
cc push action eosio.token transfer '["eosio", "maker","10000.0000 SYS","test"]' -p eosio

#4.抵押cpu 内存 买RAM

cc  system buyram maker   maker "100 SYS"   # 自己给自己买RAM
cc  system buyram eosio   maker "100 SYS"   # 别人给maker买RAM

cc system delegatebw maker maker '50.0000 SYS' '500.0000 SYS' -p maker # 自己抵押
cc system delegatebw maker maker '50.0000 SYS' '500.0000 SYS' -p maker # 别人给自己抵押



#这个命令只是eosio为maker抵押的，token处于抵押状态，不计入用户的余额状态， --transfer说抵押同时将币的所有权转给接收者，但当自己已经有所有权了，不能自己再转给自己
cc system newaccount eosio --transfer maker  \
EOS5AmFUKDGfGJVRMU4RnVgwLaCksGhH7WJVF6kQhUPJe1rwfco7d  \
EOS5AmFUKDGfGJVRMU4RnVgwLaCksGhH7WJVF6kQhUPJe1rwfco7d  \
--stake-net "10000.0000 SYS"  \
--stake-cpu "10000.0000 SYS"  \
--buy-ram-kbytes 8192

#查询
cc get account  maker
cc get currency balance eosio.token maker 
cc get table eosio eosio voters 
cc get table  maker maker order
cc get table  eosio eosio global
cc get table  eosio eosio global2


#调用合约
cc push action maker  hi '["hi liuqing"]' -p maker
cc push action maker  insert '["zhouhao","创建一个广告挂单"]' -p zhouhao
cc push action maker  getbyaccount '["zhouhao","创建一个广告挂单"]' -p zhouhao
cc push action maker  sendtoken '["zhouhao", "maker"]' -p zhouhao@active
#transaction declares authority '{"actor":"zhouhao","permission":"active"}', 
#but does not have signatures for it under a provided delay of 0 ms, provided permissions [{"actor":"maker","permission":"eosio.code"}],
#provided keys [], and a delay max limit of 3888000000 ms

cc set account permission zhouhao active '{"threshold": 1,"keys": [{"key": "EOS5YH9w2UCv7AjioR2YdBWX5cYEjTcd2hGRMEteDHr4NpWHCSaNp","weight": 1}], "accounts": [{"permission":{"actor":"maker","permission":"eosio.code"},"weight":1}]}' -p zhouhao@owner

cc set account permission test active --add-code
cc push action maker  wrapperhi '["zhouhao","创建一个广告挂单"]' -p zhouhao