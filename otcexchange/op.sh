#1.给帐号创建密钥
cc wallet create_key -n hopex

#2.创建帐号
cc system newaccount eosio otcexchange  \
EOS6xJqGjUeivzmZhNVjojmTNH4uPBb5zpJd2axqGe6o6u5RBmZrE \
EOS6xJqGjUeivzmZhNVjojmTNH4uPBb5zpJd2axqGe6o6u5RBmZrE  \
--stake-net '50.00 SYS' \
--stake-cpu '50.00 SYS' \
--buy-ram-kbytes 10000


#3.给新帐号转帐
cc push action eosio.token transfer '["eosio", "otcexchange","1.0000 SYS","test"]' -p eosio
cc push action eosio.token transfer '["eosio", "otcexchange","10000.0000 SYS","test"]' -p eosio

#4.抵押cpu 内存 买RAM

cc  system buyram otcexchange   otcexchange "100 SYS"   # 自己给自己买RAM
cc  system buyram eosio   otcexchange "100 SYS"   # 别人给maker买RAM

cc system delegatebw otcexchange otcexchange '50.0000 SYS' '500.0000 SYS' -p otcexchange # 自己抵押
cc system delegatebw otcexchange otcexchange '50.0000 SYS' '500.0000 SYS' -p otcexchange # 别人给自己抵押



#这个命令只是eosio为maker抵押的，token处于抵押状态，不计入用户的余额状态， --transfer说抵押同时将币的所有权转给接收者，但当自己已经有所有权了，不能自己再转给自己
cc system newaccount eosio --transfer otcexchange  \
EOS6xJqGjUeivzmZhNVjojmTNH4uPBb5zpJd2axqGe6o6u5RBmZrE  \
EOS6xJqGjUeivzmZhNVjojmTNH4uPBb5zpJd2axqGe6o6u5RBmZrE  \
--stake-net "10000.0000 SYS"  \
--stake-cpu "10000.0000 SYS"  \
--buy-ram-kbytes 8192

#查询
cc get account  otcexchange
cc get currency balance eosio.token otcexchange 
cc get table eosio eosio voters 
cc get table  otcexchange otcexchange order
cc get table  eosio eosio global
cc get table  eosio eosio global2
cc get table maker zhouhao order


#调用合约
cc push action otcexchange  hi '["hi liuqing"]' -p otcexchange
cc push action otcexchange  insert '["zhouhao","创建一个广告挂单"]' -p zhouhao
cc push action otcexchange  getbyaccount '["zhouhao","创建一个广告挂单"]' -p zhouhao
cc push action otcexchange  sendtoken '["zhouhao", "otcexchange"]' -p zhouhao@active
#transaction declares authority '{"actor":"zhouhao","permission":"active"}', 
#but does not have signatures for it under a provided delay of 0 ms, provided permissions [{"actor":"otcexchange","permission":"eosio.code"}],
#provided keys [], and a delay max limit of 3888000000 ms

cc set account permission zhouhao active '{"threshold": 1,"keys": [{"key": "EOS5YH9w2UCv7AjioR2YdBWX5cYEjTcd2hGRMEteDHr4NpWHCSaNp","weight": 1}], "accounts": [{"permission":{"actor":"otcexchange","permission":"eosio.code"},"weight":1}]}' -p zhouhao@owner

cc set account permission test active --add-code
cc push action otcexchange  wrapperhi '["zhouhao","创建一个广告挂单"]' -p zhouhao



cc get table otcexchange otcchange otc
cc push action otcexchange createspot '["SYS","CNY",4,4,1]' -p zhouhao
cc push action otcexchange removespots '[]' -p zhouhao //移除数据
cc push action otcexchange getspots '[]' -p zhouhao