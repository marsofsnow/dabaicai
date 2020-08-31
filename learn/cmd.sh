cc push action zhouhao adduser  '["bob1", 18588250437, "i m bob1"]' -p bob1@active
cc push action zhouhao adduser  '["abc",  18588250437, "i m abc"]' -p abc@active
cc push action zhouhao edituser '["bob", 18588250437, "i m bob new"]' -p bob@active
cc push action zhouhao loweruser '["bob"]' -p bob@active
cc push action zhouhao deleteuser '["bob1"]' -p zhouhao@active

cc push action zhouhao sendtoken '["abc", "bob"]' -p abc@active


cc get table zhouhao zhouhao user 
cc push action zhouhao addpost '["bob", "my post"]' -p bob
cc push action zhouhao addpost '["bob", "my post"]' -p bob


cc system newaccount eosio  \
--transfer alice \
EOS4uoEWHJBywcKLvCpe9THEfPMHJZGFwLTE8JCN8W8ZesUPcgrus \
EOS4uoEWHJBywcKLvCpe9THEfPMHJZGFwLTE8JCN8W8ZesUPcgrus \
--stake-net "100000000.0000 SYS" \
--stake-cpu "100000000.0000 SYS" \
--buy-ram-kbytes 8192 

cc system newaccount eosio testzhouhao  \
EOS6ddFe9ApYiyoCbW82RWzLhjYR8915ZM1PkzfuTAm7UcCxDoiFW \
EOS6ddFe9ApYiyoCbW82RWzLhjYR8915ZM1PkzfuTAm7UcCxDoiFW  \
--stake-net '50.00 SYS' \
--stake-cpu '50.00 SYS' \
--buy-ram-kbytes 10000

cc wallet create-key -n hopex

cc get currency -h


cleos push action test sendtoken '["alice", "bob"]' -p alice@active

cleos set account permission alice active permission.json -p alice@owner

{
    "threshold":1,
    "keys":[
        {
            "key":"<alice账户的公钥>",
            "weight":1
        }
    ],
    "accounts":[
        {
            "permission":{
                "actor":"test", //合约账户名
                "permission":"eosio.code"
            },
            "weight":1
        }
    ]
}


cleos push action eosio.token transfer '["zhouhao", "eosio","1.0000 SYS","vote"]' -p zhouhao


//先创建2个帐号，分别叫a1 a2

a1 EOS8MKvsVbpwmLfLnfrnpmLvDc9frrRuTJ9P5Zu32WHjHJhV4CYsz

a2 EOS6ef3Xm3LhE7mg9Hmut4scDH6zBiU1UAk6biyraDvS6Vmw6t127


cc system newaccount eosio a1  \
EOS8MKvsVbpwmLfLnfrnpmLvDc9frrRuTJ9P5Zu32WHjHJhV4CYsz \
EOS8MKvsVbpwmLfLnfrnpmLvDc9frrRuTJ9P5Zu32WHjHJhV4CYsz \
--stake-net '50.00 SYS' \
--stake-cpu '50.00 SYS' \
--buy-ram-kbytes 10000

cc system newaccount eosio a2  \
EOS6ef3Xm3LhE7mg9Hmut4scDH6zBiU1UAk6biyraDvS6Vmw6t127 \
EOS6ef3Xm3LhE7mg9Hmut4scDH6zBiU1UAk6biyraDvS6Vmw6t127  \
--stake-net '50.00 SYS' \
--stake-cpu '50.00 SYS' \
--buy-ram-kbytes 10000

//2.用zhouhao 给a1 转100SYS
cleos push action eosio.token transfer '["zhouhao", "a1","100.0000 SYS","vote"]' -p zhouhao
cc  push action eosio.token transfer '["zhouhao", "a1","100.0000 SYS","vote"]' -p zhouhao
//3.调用合约action 实现转账
cc push action zhouhao sendtoken '["a1", "a2"]' -p a1@active   //没有任何输出

//把合约部署帐号的eosio.code 权限发给action的调用者a1
cc set account permission a1 active '{"threshold": 1,"keys": [{"key": "EOS8MKvsVbpwmLfLnfrnpmLvDc9frrRuTJ9P5Zu32WHjHJhV4CYsz ","weight": 1}], "accounts": [{"permission":{"actor":"zhouhao","permission":"eosio.code"},"weight":1}]}' -p a1@owner


//再次执行
cc push action zhouhao sendtoken '["a1", "a2"]' -p a1@active 
 cc get currency balance eosio.token a1
 cc get currency balance eosio.token a2

 //on_notify
cc  set account permission zhouhao active --add-code
cc  push action eosio.token transfer '["a1","zhouhao","8.0000 SYS",""]' -p a1


//延迟交易
cc  push action zhouhao send '["a1", "for 5 seconds", 5]' -p a1