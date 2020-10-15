


cc wallet create_key -n hopex
cc wallet private_keys -n hopex

cc  create key --to-console  
cleos wallet import --private-key -n hopex

cc create account eosio  eosio.bpay  EOS5ts4iSB1vNd32U8JVuqB1NAy7y11wxc73DgWVkARjC1QyGyDso
cc create account eosio  eosio.msig  EOS5ua6JnEAjU3JsQnHxN8soUPEyEor1F8fjqG1cLMS9ijaEdPAPF
cc create account eosio  eosio.names EOS5yRKKkJ5FLSpipqLZ5jy1bi8NPHVoz5gLn7FmBYhcaHeJK1VGp
cc create account eosio  eosio.ram   EOS5zePLBASvyUhxz2cxR6aRjGksp7f96YwovQiHKM9JLFDuqKmcu
cc create account eosio  eosio.ramfee EOS66GwiAD222wacAr9BuYUMuDUg7g9aGnd6RJq6j9AickraqcUtr
cc create account eosio  eosio.saving EOS6Miaq2bd5erndswAjgotLBLvd29bRTvD3rpL9VNy6aHMhSpBeN
cc create account eosio  eosio.stake  EOS6my3Wzkh2M4eQtWKu4RsbFWAvZJ56G7CyfXKTfhpqRhvuSm9zi
cc create account eosio  eosio.token  EOS6noQW44dc7z9XcXYMWWMP6iUsSVhiqUNfWdaKgadVD31XDemLu
cc create account eosio  eosio.vpay   EOS78M4SkcsiNFybBb4qEBTUMH4YKXBtfWmLgJaWJ3ZwR41zKCYWm
cc create account eosio  eosio.rex    EOS7H4L5s6AQirFGsAuGTLwsTHNt38E6bpXgEtCDxNJZ4D3Qcgn35




#1.9 安装eosio.token

#1.10 安装eosio.msig
cc set contract eosio.token ${HOME}/dabaicai/deploy/cc/contracts1.9/eosio.token/
cc set contract eosio.msig  ${HOME}/dabaicai/deploy/cc/contracts1.9/eosio.msig/

#创建数字货币
cc push action eosio.token create '[ "eosio", "10000000000.0000 EOS" ]' -p eosio.token@active
#发行
cc push action eosio.token issue '[ "eosio", "1000000000.0000 EOS", "memo" ]' -p eosio@active

curl --request POST \
    --url http://127.0.0.1:18888/v1/producer/schedule_protocol_feature_activations \
    -d '{"protocol_features_to_activate": ["0ec7e080177b2c02b278d5088611686b49d739925a92d9bfcacd7fc6b74053bd"]}'

cc set contract eosio ${HOME}/dabaicai/deploy/cc/contracts1.8/eosio.system/ -x 1000


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


cc   set contract eosio ${HOME}/dabaicai/deploy/cc/contracts1.9/eosio.system/ -x 100




cc push action eosio setpriv '["eosio.msig", 1]' -p eosio@active
cc push action eosio init '["0", "4,EOS"]' -p eosio@active



cc system newaccount eosio --transfer accountnum11 EOS5BGsTksjoPuGrpTQGNDAD3wTvUoD8tioo4KgRjccfwEpF7HZzw --stake-net "100000000.0000 EOS" --stake-cpu "100000000.0000 EOS" --buy-ram-kbytes 8192
cc system regproducer accountnum11 EOS5BGsTksjoPuGrpTQGNDAD3wTvUoD8tioo4KgRjccfwEpF7HZzw http://accountnum11.com 

cc system newaccount eosio --transfer accountnum12 EOS5CXYetxpcMwkpTc2DaL3iurGZUPHoAKXg1QZdLrwF15JYRYUHY --stake-net "100000000.0000 EOS" --stake-cpu "100000000.0000 EOS" --buy-ram-kbytes 8192
cc system regproducer accountnum12 EOS5CXYetxpcMwkpTc2DaL3iurGZUPHoAKXg1QZdLrwF15JYRYUHY http://accountnum12.com 

cc system newaccount eosio --transfer accountnum13 EOS5kRFTVqLMk8Cc5oTzdriN2hnat87AsmmujgYZg7rWGG1MJkJFW --stake-net "100000000.0000 EOS" --stake-cpu "100000000.0000 EOS" --buy-ram-kbytes 8192
cc system regproducer accountnum13 EOS5kRFTVqLMk8Cc5oTzdriN2hnat87AsmmujgYZg7rWGG1MJkJFW http://accountnum13.com 

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




cc system newaccount eosio --transfer eosio.bpay  EOS5ts4iSB1vNd32U8JVuqB1NAy7y11wxc73DgWVkARjC1QyGyDso --stake-net "100000000.0000 EOS" --stake-cpu "100000000.0000 EOS" --buy-ram-kbytes 8192