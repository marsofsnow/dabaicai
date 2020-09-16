cc  create key --to-console
Private key: 5KYdgMCMU2M16qLS9bPu1skEd41ZH1T8eL4kkLRpm4aidtt15iF
Public key: EOS7yfKrraZgMwzHWb3w97d4zDu7idyGRkuRdKAxhxBfuhCY6WVus

cc wallet import --private-key -n zyy



---------------------------

#1.eosio.bpay
cc  create key --to-console 
Private key: 5HyVXKQCx8qAbtEbGHZLeaSkiGRGpaM9xSRSvhxn99du1B1rgDK
Public key: EOS6x4PM2xXq6UNqinLQCMYUAaBfaBzSuGQGx46UUZHboSHdmQgmF

cc create account eosio eosio.bpay EOS6x4PM2xXq6UNqinLQCMYUAaBfaBzSuGQGx46UUZHboSHdmQgmF



#2.eosio.msig
c  create key --to-console 
Private key: 5JqN2YrTLnRYJqMEMtMt4PKCcbBiP8PhPu1KiLhLsjABaH6NPKw
Public key: EOS4uc8F8Hjo2HVTpdseztwq7xE6zMYZciFRUXiNcaXp5SJzFDchC
cc create account eosio eosio.msig EOS4uc8F8Hjo2HVTpdseztwq7xE6zMYZciFRUXiNcaXp5SJzFDchC


3.eosio.names
cc  create key --to-console 
Private key: 5J2YoiuuifMjGAvXpMiMyRbcnTiRpDNnNdd1PDyv9WW24kqAwrP
Public key: EOS62rAV9A8X1nD8Un5uGhdmUR7uJDGMQrzaYm9V3ASRoGuPYU3HV

cc create account eosio eosio.names EOS62rAV9A8X1nD8Un5uGhdmUR7uJDGMQrzaYm9V3ASRoGuPYU3HV


4.eosio.ram
cc  create key --to-console 
Private key: 5JASicy8uwT1bLgqYvgd6uoW3f9jf74TFFKh5DUdEwbkhfT3Ud8
Public key: EOS5yXHuychDwksa8Ykdz5bpy4oTSnpy37Y67E2sDhdujb5YgNn7c
cc create account eosio eosio.ram EOS5yXHuychDwksa8Ykdz5bpy4oTSnpy37Y67E2sDhdujb5YgNn7c


#5.eosio.ramfee
cc  create key --to-console 
Private key: 5JEuGBUQF46LaLRzn5YKg2YLnwnqyP5z5vTPCDtwjkrGLJ9rG1D
Public key: EOS5oAnggMB9yEVqew2amtuyRnorUFFour8qiJFXmLLmzsrdniD56
cc create account eosio eosio.ramfee EOS5oAnggMB9yEVqew2amtuyRnorUFFour8qiJFXmLLmzsrdniD56

#6.eosio.saving

cc  create key --to-console 
Private key: 5KH5HwJovSC9MwmQfi3u4mTw8GPaWeSVyoz7sM3VRZqwcNUuFtr
Public key: EOS5JwjJNnY2x9WTkvw5hHdmgJRBpmS1ywmMC1HLnnsSRV9XCd1sJ
cc create account eosio eosio.saving EOS5JwjJNnY2x9WTkvw5hHdmgJRBpmS1ywmMC1HLnnsSRV9XCd1sJ

#7.eosio.stake
cc  create key --to-console 
Private key: 5JbMboWKs1APtrkXR36C8mj8t4EhVoL4icUYH368FRFwrBNT4tT
Public key: EOS7rDAJKG9zcp8zFURhTrTDW5zs2ktXBVN8eAerdLEDSw1esYYyk
cc create account eosio eosio.stake EOS7rDAJKG9zcp8zFURhTrTDW5zs2ktXBVN8eAerdLEDSw1esYYyk

#8.eosio.token
cc  create key --to-console 
Private key: 5J8JVrEaBdbxXs8qufXmqJL8N891iWDt3HMzKWPCeGy5iBaxXxy
Public key: EOS5pkEJapfQcCrnMHgoGoVEZyxucwa2rTD7jac6cmiGb2hzrrsSg

cc create account eosio eosio.token EOS5pkEJapfQcCrnMHgoGoVEZyxucwa2rTD7jac6cmiGb2hzrrsSg

#9.eosio.vpay
cc  create key --to-console 
Private key: 5KgwLZD1AkzgoHWWTDfqCQf7RvmrzuexLmAFoCmvRxi4ouy3ycW
Public key: EOS6y5QTwJxNKyLQCqEyUFnLT3jvKgLj3q5Jsk1rNkFgMrLBav4aB
cc create account eosio eosio.vpay EOS6y5QTwJxNKyLQCqEyUFnLT3jvKgLj3q5Jsk1rNkFgMrLBav4aB
#10.eosio.rex
Private key: 5JYWoChqTUxwMeVtyYdJii6gKnXtWpkB3ux4M3Wt7P1SLNKmiux
Public key: EOS7Ph7sdxhUasGFYPzKp46DwD3Sy3Z8Fohxe4su5JQphurVBC27H
cc create account eosio eosio.rex EOS7Ph7sdxhUasGFYPzKp46DwD3Sy3Z8Fohxe4su5JQphurVBC27H

#1.9 安装eosio.token
cc set contract eosio.token /home/zh/eosio/cc/contracts1.9/eosio.token/
#1.10 安装eosio.msig
cc set contract eosio.msig /home/zh/eosio/cc/contracts1.9/eosio.msig/

#创建数字货币
cc push action eosio.token create '[ "eosio", "10000000000.0000 SYS" ]' -p eosio.token@active
#发行
cc push action eosio.token issue '[ "eosio", "1000000000.0000 SYS", "memo" ]' -p eosio@active

curl --request POST \
    --url http://127.0.0.1:18888/v1/producer/schedule_protocol_feature_activations \
    -d '{"protocol_features_to_activate": ["0ec7e080177b2c02b278d5088611686b49d739925a92d9bfcacd7fc6b74053bd"]}'

cc set contract eosio /home/zh/eosio/cc/contracts1.8/eosio.system/ -x 1000


# GET_SENDER
cc push action eosio activate '["f0af56d2c5a48d60a4a5b5c903edfb7db3a736a94ed589d0b797df33ff9d3e1d"]' -p eosio

# FORWARD_SETCODE
cc push action eosio activate '["2652f5f96006294109b3dd0bbde63693f55324af452b799ee137a81a905eed25"]' -p eosio

# ONLY_BILL_FIRST_AUTHORIZER
cc push action eosio activate '["8ba52fe7a3956c5cd3a656a3174b931d3bb2abb45578befc59f283ecd816a405"]' -p eosio

# RESTRICT_ACTION_TO_SELF
cc push action eosio activate '["ad9e3d8f650687709fd68f4b90b41f7d825a365b02c23a636cef88ac2ac00c43"]' -p eosio

# DISALLOW_EMPTY_PRODUCER_SCHEDULE
cc push action eosio activate '["68dcaa34c0517d19666e6b33add67351d8c5f69e999ca1e37931bc410a297428"]' -p eosio

 # FIX_LINKAUTH_RESTRICTION
cc push action eosio activate '["e0fb64b1085cc5538970158d05a009c24e276fb94e1a0bf6a528b48fbc4ff526"]' -p eosio

 # REPLACE_DEFERRED
cc push action eosio activate '["ef43112c6543b88db2283a2e077278c315ae2c84719a8b25f25cc88565fbea99"]' -p eosio

# NO_DUPLICATE_DEFERRED_ID
cc push action eosio activate '["4a90c00d55454dc5b059055ca213579c6ea856967712a56017487886a4d4cc0f"]' -p eosio

# ONLY_LINK_TO_EXISTING_PERMISSION
cc push action eosio activate '["1a99a59d87e06e09ec5b028a9cbb7749b4a5ad8819004365d02dc4379a8b7241"]' -p eosio

# RAM_RESTRICTIONS
cc push action eosio activate '["4e7bf348da00a945489b2a681749eb56f5de00b900014e137ddae39f48f69d67"]' -p eosio

# WEBAUTHN_KEY
cc push action eosio activate '["4fca8bd82bbd181e714e283f83e1b45d95ca5af40fb89ad3977b653c448f78c2"]' -p eosio

# WTMSIG_BLOCK_SIGNATURES
cc push action eosio activate '["299dcb6af692324b899b39f16d5a530a33062804e41f09dc97e9f156b4476707"]' -p eosio


cc   set contract eosio /home/zh/eosio/cc/contracts1.9/eosio.system/ -x 100


cc push action eosio setpriv '["eosio.msig", 1]' -p eosio@active
cc push action eosio init '["0", "4,SYS"]' -p eosio@active


cc create key --to-console
cc create key --to-console

--signature-provider EOS8BvSZgN2hDpAuT3gF6htxmG1Zt8jijYk2SR8cWZcpk28Y7GmtL=KEY:5KQ8ZGgYf3w6WJsambh5QzQNZHKwwWD58EcLSKmVX3C9tfNTU8v \
Private key: 5KQ8ZGgYf3w6WJsambh5QzQNZHKwwWD58EcLSKmVX3C9tfNTU8v
Public key: EOS8BvSZgN2hDpAuT3gF6htxmG1Zt8jijYk2SR8cWZcpk28Y7GmtL

cc wallet import --private-key -n zyy

cc system newaccount eosio --transfer accountnum11 EOS8BvSZgN2hDpAuT3gF6htxmG1Zt8jijYk2SR8cWZcpk28Y7GmtL --stake-net "100000000.0000 SYS" --stake-cpu "100000000.0000 SYS" --buy-ram-kbytes 8192
cc system regproducer accountnum11 EOS8BvSZgN2hDpAuT3gF6htxmG1Zt8jijYk2SR8cWZcpk28Y7GmtL https://accountnum11.com 





cc create key --to-console
Private key: 5K2VZ7NoRXnsT4kAZhe4msGWCJxmd21Fr87KqMYoa91Bshu8hdR
Public key: EOS5kP5kFHij8BjDJBhFb4ADTgXber76kSKR215Zb2tZDfrnjVfhk

--signature-provider EOS5kP5kFHij8BjDJBhFb4ADTgXber76kSKR215Zb2tZDfrnjVfhk=KEY:5K2VZ7NoRXnsT4kAZhe4msGWCJxmd21Fr87KqMYoa91Bshu8hdR \




cc system newaccount eosio --transfer accountnum11 EOS8BvSZgN2hDpAuT3gF6htxmG1Zt8jijYk2SR8cWZcpk28Y7GmtL --stake-net "100000000.0000 SYS" --stake-cpu "100000000.0000 SYS" --buy-ram-kbytes 8192
cc system regproducer accountnum11 EOS8BvSZgN2hDpAuT3gF6htxmG1Zt8jijYk2SR8cWZcpk28Y7GmtL https://accountnum11.com 


cc system newaccount eosio --transfer accountnum12 EOS5kP5kFHij8BjDJBhFb4ADTgXber76kSKR215Zb2tZDfrnjVfhk --stake-net "100000000.0000 SYS" --stake-cpu "100000000.0000 SYS" --buy-ram-kbytes 8192
cc system regproducer accountnum12 EOS5kP5kFHij8BjDJBhFb4ADTgXber76kSKR215Zb2tZDfrnjVfhk https://accountnum12.com 


cc system newaccount eosio --transfer accountnum13 EOS62fodgaQK7gY3juDpC9jP3mgLmdZ9RpzeAbSNyEgVshnaJGXGu --stake-net "100000000.0000 SYS" --stake-cpu "100000000.0000 SYS" --buy-ram-kbytes 8192
cc system regproducer accountnum13 EOS62fodgaQK7gY3juDpC9jP3mgLmdZ9RpzeAbSNyEgVshnaJGXGu https://accountnum13.com 


cc system voteproducer prods accountnum11 accountnum11 accountnum12 accountnum13



cc create key --to-console
Private key: 5JeXzc4SSLSZefbbt3obJ3XcbVhLUoW81dgyJn5icXJZGBTFreV
Public key: EOS62fodgaQK7gY3juDpC9jP3mgLmdZ9RpzeAbSNyEgVshnaJGXGu


--signature-provider EOS62fodgaQK7gY3juDpC9jP3mgLmdZ9RpzeAbSNyEgVshnaJGXGu=KEY:5JeXzc4SSLSZefbbt3obJ3XcbVhLUoW81dgyJn5icXJZGBTFreV \



cc system newaccount eosio --transfer accountnum13 EOS62fodgaQK7gY3juDpC9jP3mgLmdZ9RpzeAbSNyEgVshnaJGXGu --stake-net "100000000.0000 SYS" --stake-cpu "100000000.0000 SYS" --buy-ram-kbytes 8192
cc system regproducer accountnum13 EOS62fodgaQK7gY3juDpC9jP3mgLmdZ9RpzeAbSNyEgVshnaJGXGu https://accountnum13.com 




cc system voteproducer prods accountnum11 accountnum11 accountnum12 accountnum13


cc push action eosio updateauth '{"account": "eosio", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio.prods", "permission": "active"}}]}}' -p eosio@owner
cc push action eosio updateauth '{"account": "eosio", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio.prods", "permission": "active"}}]}}' -p eosio@active

cc push action eosio updateauth '{"account": "eosio.bpay", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.bpay@owner
cc push action eosio updateauth '{"account": "eosio.bpay", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.bpay@active

cc push action eosio updateauth '{"account": "eosio.msig", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.msig@owner
cc push action eosio updateauth '{"account": "eosio.msig", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.msig@active

cc push action eosio updateauth '{"account": "eosio.names", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.names@owner
cc push action eosio updateauth '{"account": "eosio.names", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.names@active

cc push action eosio updateauth '{"account": "eosio.ram", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.ram@owner
cc push action eosio updateauth '{"account": "eosio.ram", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.ram@active

cc push action eosio updateauth '{"account": "eosio.ramfee", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.ramfee@owner
cc push action eosio updateauth '{"account": "eosio.ramfee", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.ramfee@active

cc push action eosio updateauth '{"account": "eosio.saving", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.saving@owner
cc push action eosio updateauth '{"account": "eosio.saving", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.saving@active

cc push action eosio updateauth '{"account": "eosio.stake", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.stake@owner
cc push action eosio updateauth '{"account": "eosio.stake", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.stake@active

cc push action eosio updateauth '{"account": "eosio.token", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.token@owner
cc push action eosio updateauth '{"account": "eosio.token", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.token@active

cc push action eosio updateauth '{"account": "eosio.vpay", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.vpay@owner
cc push action eosio updateauth '{"account": "eosio.vpay", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.vpay@active