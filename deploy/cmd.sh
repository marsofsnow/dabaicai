cc get info


cc  create key --to-console 
# owner
Private key: 5JAFYvsrB1GwCTeNdZkyE2dBhRLVMUemB1pY1LEQa7ShgFLAS5G
Public key: EOS6tk5sW7BTbDb8daRdZvaUzJ7TJnHDvtsMxHXSAFaJbCvnzkaKN

#active
zh@zh-Vostro-5880:~/eosio/eos/deploy$ cc  create key --to-console 
Private key: 5JASBgCzMRTZqyDHLTzDgT3CkxZ3RSr8mKs1TDJjb4q5dkbSGpG
Public key: EOS8Md4CvaVW9WZzisKxHWPzKppsG9YQ3Fmrz9UJotSGcHPm5RyAq

#custom
zh@zh-Vostro-5880:~/eosio/eos/deploy$ cc  create key --to-console 
Private key: 5KUzV7rv66GKVEPKKAh8d2UfPVbyiFg2bY7YkY5bLwkvxVN5dfk
Public key: EOS6mymDfqEbgzGt3QQ3n2VayZR1bf3CPrqTBDGtufCZ9A497mEvM


 cc  create key --to-console 
 Private key: 5KhPcooHi8HfVwq9zVDsSaLpSMewBgrYyhdnxsrKVHue34Pxqes
Public key: EOS5YH9w2UCv7AjioR2YdBWX5cYEjTcd2hGRMEteDHr4NpWHCSaNp
zh@zh-Vostro-5880:~/eosio/eos/deploy$ ^C
zh@zh-Vostro-5880:~/eosio/eos/deploy$ cc  create key --to-console 
Private key: 5KhEz1UH64GioVLuGqbqwnzFQzPMNNLGRBu338ufRWu1U9EMstF
Public key: EOS7GUqaiJb2raAy22utJzUyLQKUNGHFFrWLodMQ7JG7FDnHVAdgz

echo "5KhPcooHi8HfVwq9zVDsSaLpSMewBgrYyhdnxsrKVHue34Pxqes" | cc wallet import --private-key -n hopex
echo "5KhEz1UH64GioVLuGqbqwnzFQzPMNNLGRBu338ufRWu1U9EMstF" | cc wallet import --private-key -n hopex

echo "5KUzV7rv66GKVEPKKAh8d2UfPVbyiFg2bY7YkY5bLwkvxVN5dfk" | cc wallet import --private-key -n hopex
echo "5JASBgCzMRTZqyDHLTzDgT3CkxZ3RSr8mKs1TDJjb4q5dkbSGpG" | cc wallet import --private-key -n hopex
echo "5JAFYvsrB1GwCTeNdZkyE2dBhRLVMUemB1pY1LEQa7ShgFLAS5G" | cc wallet import --private-key -n hopex

cc system newaccount eosio bcskill EOS6tk5sW7BTbDb8daRdZvaUzJ7TJnHDvtsMxHXSAFaJbCvnzkaKN EOS8Md4CvaVW9WZzisKxHWPzKppsG9YQ3Fmrz9UJotSGcHPm5RyAq  --stake-net '50.00 SYS' --stake-cpu '50.00 SYS' --buy-ram-kbytes 10000

cc get account bcskill -j
{
  "account_name": "bcskill",
  "head_block_num": 7156,
  "head_block_time": "2020-08-26T05:52:11.000",
  "privileged": false,
  "last_code_update": "1970-01-01T00:00:00.000",
  "created": "2020-08-26T05:46:41.000",
  "ram_quota": 10241390,
  "net_weight": 500000,
  "cpu_weight": 500000,
  "net_limit": {
    "used": 0,
    "available": 30198983,
    "max": 30198983
  },
  "cpu_limit": {
    "used": 0,
    "available": 2879999,
    "max": 2879999
  },
  "ram_usage": 2996,
  "permissions": [{
      "perm_name": "active",
      "parent": "owner",
      "required_auth": {
        "threshold": 1,
        "keys": [{
            "key": "EOS8Md4CvaVW9WZzisKxHWPzKppsG9YQ3Fmrz9UJotSGcHPm5RyAq",
            "weight": 1
          }
        ],
        "accounts": [],
        "waits": []
      }
    },{
      "perm_name": "owner",
      "parent": "",
      "required_auth": {
        "threshold": 1,
        "keys": [{
            "key": "EOS6tk5sW7BTbDb8daRdZvaUzJ7TJnHDvtsMxHXSAFaJbCvnzkaKN",
            "weight": 1
          }
        ],
        "accounts": [],
        "waits": []
      }
    }
  ],
  "total_resources": {
    "owner": "bcskill",
    "net_weight": "50.0000 SYS",
    "cpu_weight": "50.0000 SYS",
    "ram_bytes": 10239990
  },
  "self_delegated_bandwidth": null,
  "refund_request": null,
  "voter_info": null,
  "rex_info": null
}




.为bcskill账号增加自定义权限组custom，基于active
cc set account permission bcskill custom '{"threshold":1,"keys":[{"key":"EOS6mymDfqEbgzGt3QQ3n2VayZR1bf3CPrqTBDGtufCZ9A497mEvM","weight":1}],"accounts":[]}' active

cc set account permission bcskill custom '{"threshold":1,"keys":[{"key": "EOS5YH9w2UCv7AjioR2YdBWX5cYEjTcd2hGRMEteDHr4NpWHCSaNp", "weight": 1}], "accounts": []}' active
cc set account permission bcskill custom '{"threshold":1,"keys":[{"key": "EOS5YH9w2UCv7AjioR2YdBWX5cYEjTcd2hGRMEteDHr4NpWHCSaNp", "weight": 1},{"key": "EOS6mymDfqEbgzGt3QQ3n2VayZR1bf3CPrqTBDGtufCZ9A497mEvM", "weight":1}],"accounts":[]}' active

cc system newaccount eosio temporary EOS7GUqaiJb2raAy22utJzUyLQKUNGHFFrWLodMQ7JG7FDnHVAdgz EOS7GUqaiJb2raAy22utJzUyLQKUNGHFFrWLodMQ7JG7FDnHVAdgz  --stake-net '50.00 SYS' --stake-cpu '50.00 SYS' --buy-ram-kbytes 10000


cc system newaccount eosio bob EOS5fvzB3Tf3SUEWeBaHu2nFjLqr9oRKHkycypouWWbQ2HGQAWuUT EOS5fvzB3Tf3SUEWeBaHu2nFjLqr9oRKHkycypouWWbQ2HGQAWuUT  --stake-net '50.00 SYS' --stake-cpu '50.00 SYS' --buy-ram-kbytes 10000


cc system newaccount eosio bob1 EOS7hxrnKmZibuGModS7Pc238BQARYUqyaCVULHDBhtHMPnVSYdmH EOS7hxrnKmZibuGModS7Pc238BQARYUqyaCVULHDBhtHMPnVSYdmH  --stake-net '50.00 SYS' --stake-cpu '50.00 SYS' --buy-ram-kbytes 10000


cc system newaccount eosio abc EOS6xcyEutJugEUV1vUi3FTc5XQ4bMZHZHVR89Q6WFa8FqhJUpWoN EOS6xcyEutJugEUV1vUi3FTc5XQ4bMZHZHVR89Q6WFa8FqhJUpWoN  --stake-net '50.00 SYS' --stake-cpu '50.00 SYS' --buy-ram-kbytes 10000




cc set account permission bcskill custom '{"threshold":2,"keys":[{"key":"EOS6mymDfqEbgzGt3QQ3n2VayZR1bf3CPrqTBDGtufCZ9A497mEvM","weight":1}],"accounts":[{"permission":{"actor":"temporary","permission":"active"},"weight":1}]}' active

cc set account permission bcskill custom 'NULL' active


cc push action eosio.token transfer '["eosio", "bcskill","1000.0000 SYS","bcskill"]' -p eosio
cc get currency balance eosio.token bcskill

cc push action eosio.token transfer '["bcskill", "eosio","100.0000 SYS","bcskill"]' -p bcskill@custom
cc set action permission bcskill eosio.token transfer custom

#cleos system delegatebw voter1 voter2 '25000000.0000 SYS' '25000000.0000 SYS' --transfer

#cc get table eosio eosio rammarket



cc system newaccount eosio zhouhao EOS5YH9w2UCv7AjioR2YdBWX5cYEjTcd2hGRMEteDHr4NpWHCSaNp  EOS5YH9w2UCv7AjioR2YdBWX5cYEjTcd2hGRMEteDHr4NpWHCSaNp  --stake-net '0.00 SYS' --stake-cpu '0.00 SYS'  --buy-ram-kbytes 3
cc push action eosio.token transfer '["eosio", "zhouhao","100.0000 SYS","vote"]' -p eosio
cc push action eosio.token transfer '["zhouhao", "eosio","1.0000 SYS","vote"]' -p zhouhao@active

Private key: 5JcFb5wTtNkhRLrVURgtdbHrAPPAfWUaDy36CHznstNwsxqWGUA
Public key: EOS8NSFzdxgcFKvDEGg2AqNA8Gq57KnNGEoT2vfJXteTyJmF9bkLF

#帐号转移

#转移active权限
cc push action eosio updateauth '{"permission":"active","parent":"owner","account":"zhouhao","auth": {"accounts": [], "waits": [], "keys": [{"key": "EOS8NSFzdxgcFKvDEGg2AqNA8Gq57KnNGEoT2vfJXteTyJmF9bkLF", "weight": 1}], "threshold": 1}}' -p zhouhao@active

#转移owner权限
cc push action eosio updateauth '{"permission":"owner","parent":"","account":"zhouhao","auth": {"accounts": [], "waits": [], "keys": [{"key": "EOS8NSFzdxgcFKvDEGg2AqNA8Gq57KnNGEoT2vfJXteTyJmF9bkLF", "weight": 1}], "threshold": 1}}' -p zhouhao@owner


# 自己买RAM
cc  system buyram zhouhao zhouhao "100 SYS"

# 让别人帮我买
cc system buyram eosio zhouhao "100 SYS"

#自己抵押获取cpu net
 cc system delegatebw zhouhao zhouhao '50.0000 SYS' '500.0000 SYS' -p zhouhao
 cc system delegatebw zhouhao zhouhao '10000000.0000 SYS' '10000000.0000 SYS' --transfer
 delegatebw 自己给自己抵押时报 Error 3050003: eosio_assert_message assertion failure 抵押时不能再添加 --transfer（此参数添加时，是说抵押同时将币的所有权转给接收者，但自己已经有所有权了，不能自己再转给自己）

# 转账
cleos push action eosio.token transfer '["zhouhao", "eosio","1.0000 SYS","vote"]' -p zhouhao


curl --request POST  --url http://127.0.0.1：18888/get_controlled_accounts  --data '{"controlling_account":"eosio"}'