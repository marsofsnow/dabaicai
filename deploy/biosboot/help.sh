




#1.eosio.bpay
cleos create key --to-console
Private key: 5KF7Fh8HH2UUKVex2BYmrSLMiZqLPxT1ib2AVctiuqYF47Xzxk7
Public key: EOS5FLFMYhweNhPFq36fF6E3hMiKJdAuWQUJfPAXspPGRRktfCjST


cleos wallet import --private-key -n zyy

cleos --url http://127.0.0.1:18888  create account eosio eosio.bpay EOS5FLFMYhweNhPFq36fF6E3hMiKJdAuWQUJfPAXspPGRRktfCjST 

xecuted transaction: 62a5fc5890e69a1d353215fa8dc36190b7264447d3e6a5ce5fea409d97454d81  200 bytes  462 us
#         eosio <= eosio::newaccount            {"creator":"eosio","name":"eosio.bpay","owner":{"threshold":1,"keys":[{"key":"EOS5FLFMYhweNhPFq36fF6...


#2.eosio.msig
cleos create key --to-console
Private key: 5HudzmijqnT6N6EYJ88ohkUdERMzFHpfHsbZezYPEA5StLrmyxv
Public key: EOS6aswEH7np7APWSAVanY7achzzsTwN3om75pgdGAMQEf6z1C3pJ
cleos --url http://127.0.0.1:18888  create account eosio eosio.msig EOS6aswEH7np7APWSAVanY7achzzsTwN3om75pgdGAMQEf6z1C3pJ 

#3. eosio.names
cleos create key --to-console
Private key: 5JSh5VsNh7AhP58UgAXuoudqR2R7JACVFYoEckL6vCRWNJGTvP2
Public key: EOS5iGLM5BXW7z5ryZHzWSbsoxVc8KP2xxJrWp8EcAboDb5y2tKgk
cleos wallet import --private-key -n zyy
cleos --url http://127.0.0.1:18888  create account eosio  eosio.names EOS5iGLM5BXW7z5ryZHzWSbsoxVc8KP2xxJrWp8EcAboDb5y2tKgk


#4.eosio.ram
cleos create key --to-console
Private key: 5JZtWdo6qpMC3cFN53sSp1n4XbSe593YBhWVSB3gW9hUfCYDQ8z
Public key: EOS59UvotwBtg4KwPPfbVMDU2UFSMGckK6NjmALAkim9uuua4qQkC
cleos wallet import --private-key -n zyy
cleos --url http://127.0.0.1:18888  create account eosio  eosio.ram EOS59UvotwBtg4KwPPfbVMDU2UFSMGckK6NjmALAkim9uuua4qQkC



#5.eosio.ramfee
cleos create key --to-console
Private key: 5Ke1sjmMBrmbiKS9iSVFNFPgJSUhXjjbPs97nf5VDdzLw1rWMsD
Public key: EOS628Bbh5Xw5WhCzt8FUxaBYohvtH5UqxK8Rn3UBUH48WHXWzNAk
cleos wallet import --private-key -n zyy
cleos --url http://127.0.0.1:18888  create account eosio eosio.ramfee EOS628Bbh5Xw5WhCzt8FUxaBYohvtH5UqxK8Rn3UBUH48WHXWzNAk

#6.eosio.saving
cleos create key --to-console
Private key: 5KVXEytQ6ndmNJ96SmQ5Kdef1zA3kza13NELRw2fFBM9JbGPHg3
Public key: EOS5Rq3aDi3N5qjNktAb1CZUKYWrxxHSeBkDFinZjy81Zfhj1WrW5
cleos wallet import --private-key -n zyy
cleos --url http://127.0.0.1:18888  create account eosio eosio.saving EOS5Rq3aDi3N5qjNktAb1CZUKYWrxxHSeBkDFinZjy81Zfhj1WrW5

#7.eosio.stake
cleos create key --to-console
Private key: 5J2CiEKznnTseD1GSVZ76n8LFAmmsUZNC2Rf3aq68PLcXvkr4Pw
Public key: EOS6rnFK9FpVC2JNtD2rUyLeDf14qpGBH1xJdNR8KvU9K4gwFZu1i
cleos wallet import --private-key -n zyy
cleos --url http://127.0.0.1:18888  create account eosio eosio.stake EOS6rnFK9FpVC2JNtD2rUyLeDf14qpGBH1xJdNR8KvU9K4gwFZu1i

#8.eosio.token
cleos create key --to-console
Private key: 5JaKBmktscbsas8fxqjrQnx8bncAeqQAF1JSFRXktkYg7RHv6FR
Public key: EOS6zKKyCy4JSiX4RsDfvBqEvrYL7BBBdBHXVu3d5vmp82LCdT2Ds
cleos wallet import --private-key -n zyy
cleos --url http://127.0.0.1:18888  create account eosio eosio.token EOS6zKKyCy4JSiX4RsDfvBqEvrYL7BBBdBHXVu3d5vmp82LCdT2Ds


#9.eosio.vpay
cleos create key --to-console
Private key: 5JAC7ZjrwkvsaWd7avgqMFb7R53AJjE66kiTFwCTvSUo5Lxm6oX
Public key: EOS5ddMsZ7z5nYSaR9stzaP9JDxNBwB1f21mPx5AJ5H9AAGj6yXZx
cleos wallet import --private-key -n zyy
cleos --url http://127.0.0.1:18888  create account eosio eosio.vpay EOS5ddMsZ7z5nYSaR9stzaP9JDxNBwB1f21mPx5AJ5H9AAGj6yXZx

#10.eosio.rex

cleos create key --to-console
Private key: 5KMjjsgLTLx1LRqaWXpwSqfLLiCAnbnDx3jUy2R3U9oYgjhybWi
Public key: EOS8JjoWaG7UKohgtmWDmN4hnDpqhwbu4YiHmybtH3rZttYRtTmaT
cleos wallet import --private-key -n zyy
cleos --url http://127.0.0.1:18888  create account eosio eosio.rex EOS8JjoWaG7UKohgtmWDmN4hnDpqhwbu4YiHmybtH3rZttYRtTmaT



cleos --url http://127.0.0.1:18888 set contract eosio.token /home/zh/eosio/cc/contracts1.9/eosio.token/
cleos --url http://127.0.0.1:18888 set contract eosio.msig /home/zh/eosio/cc/contracts1.9/eosio.msig/


cleos --url http://127.0.0.1:18888 push action eosio.token create '[ "eosio", "10000000000.0000 HOPEX" ]' -p eosio.token@active

cleos --url http://127.0.0.1:18888 push action eosio.token issue '[ "eosio", "1000000000.0000 HOPEX", "memo" ]' -p eosio@active


curl --request POST \
    --url http://127.0.0.1:18888/v1/producer/schedule_protocol_feature_activations \
    -d '{"protocol_features_to_activate": ["0ec7e080177b2c02b278d5088611686b49d739925a92d9bfcacd7fc6b74053bd"]}'



cleos --url http://127.0.0.1:18888  set contract eosio /home/zh/eosio/cc/contracts1.8/eosio.system/ -x 1000


# GET_SENDER
cleos --url http://127.0.0.1:18888 push action eosio activate '["f0af56d2c5a48d60a4a5b5c903edfb7db3a736a94ed589d0b797df33ff9d3e1d"]' -p eosio

# FORWARD_SETCODE
cleos --url http://127.0.0.1:18888 push action eosio activate '["2652f5f96006294109b3dd0bbde63693f55324af452b799ee137a81a905eed25"]' -p eosio

# ONLY_BILL_FIRST_AUTHORIZER
cleos --url http://127.0.0.1:18888 push action eosio activate '["8ba52fe7a3956c5cd3a656a3174b931d3bb2abb45578befc59f283ecd816a405"]' -p eosio

# RESTRICT_ACTION_TO_SELF
cleos --url http://127.0.0.1:18888 push action eosio activate '["ad9e3d8f650687709fd68f4b90b41f7d825a365b02c23a636cef88ac2ac00c43"]' -p eosio

# DISALLOW_EMPTY_PRODUCER_SCHEDULE
cleos --url http://127.0.0.1:18888 push action eosio activate '["68dcaa34c0517d19666e6b33add67351d8c5f69e999ca1e37931bc410a297428"]' -p eosio

 # FIX_LINKAUTH_RESTRICTION
cleos --url http://127.0.0.1:18888 push action eosio activate '["e0fb64b1085cc5538970158d05a009c24e276fb94e1a0bf6a528b48fbc4ff526"]' -p eosio

 # REPLACE_DEFERRED
cleos --url http://127.0.0.1:18888 push action eosio activate '["ef43112c6543b88db2283a2e077278c315ae2c84719a8b25f25cc88565fbea99"]' -p eosio

# NO_DUPLICATE_DEFERRED_ID
cleos --url http://127.0.0.1:18888 push action eosio activate '["4a90c00d55454dc5b059055ca213579c6ea856967712a56017487886a4d4cc0f"]' -p eosio

# ONLY_LINK_TO_EXISTING_PERMISSION
cleos --url http://127.0.0.1:18888 push action eosio activate '["1a99a59d87e06e09ec5b028a9cbb7749b4a5ad8819004365d02dc4379a8b7241"]' -p eosio

# RAM_RESTRICTIONS
cleos --url http://127.0.0.1:18888 push action eosio activate '["4e7bf348da00a945489b2a681749eb56f5de00b900014e137ddae39f48f69d67"]' -p eosio

# WEBAUTHN_KEY
cleos --url http://127.0.0.1:18888 push action eosio activate '["4fca8bd82bbd181e714e283f83e1b45d95ca5af40fb89ad3977b653c448f78c2"]' -p eosio

# WTMSIG_BLOCK_SIGNATURES
cleos --url http://127.0.0.1:18888 push action eosio activate '["299dcb6af692324b899b39f16d5a530a33062804e41f09dc97e9f156b4476707"]' -p eosio

cleos --url http://127.0.0.1:18888  set contract eosio /home/zh/eosio/cc/contracts1.9/eosio.system/ -x 1000


cleos --url http://127.0.0.1:18888 push action eosio setpriv '["eosio.msig", 1]' -p eosio@active

cleos --url http://127.0.0.1:18888 push action eosio init '["0", "4,HOPEX"]' -p eosio@active

cleos ---url http://127.0.0.1:18888  system listproducers


curl -X POST http://127.0.0.1:18888/v1/chain/get_activated_protocol_features -d '{}' | jq



cleos --url http://127.0.0.1:18888  system listproducers





cleos create key --to-console
Private key: 5Hr88BwftJx7hpDAr86BMfitBnvDb8KcBsYfWTNqFAjZozzTfTX
Public key: EOS5dM8j4m4Dh9zUzAaGy1DxXHC73HNFd9p93B4mVpg9b6TuNX8DK
cleos wallet import --private-key -n zyy

cleos  --url http://127.0.0.1:18888 system newaccount eosio --transfer accountnum11 EOS5dM8j4m4Dh9zUzAaGy1DxXHC73HNFd9p93B4mVpg9b6TuNX8DK --stake-net "100000000.0000 HOPEX" --stake-cpu "100000000.0000 HOPEX" --buy-ram-kbytes 8192


cleos  --url http://127.0.0.1:18888 system newaccount eosio --transfer accountnum11 EOS5dM8j4m4Dh9zUzAaGy1DxXHC73HNFd9p93B4mVpg9b6TuNX8DK --stake-net "100000000.0000 HOPEX" --stake-cpu "100000000.0000 HOPEX" --buy-ram-kbytes 8192

executed transaction: 424a99ac6bec8395192aa360f1ea1e116bf548e73be96064c740c7a1a17dd522  336 bytes  767 us
#         eosio <= eosio::newaccount            {"creator":"eosio","name":"accountnum11","owner":{"threshold":1,"keys":[{"key":"EOS5dM8j4m4Dh9zUzAaG...
#         eosio <= eosio::buyrambytes           {"payer":"eosio","receiver":"accountnum11","bytes":8388608}
#         eosio <= eosio::delegatebw            {"from":"eosio","receiver":"accountnum11","stake_net_quantity":"100000000.0000 HOPEX","stake_cpu_qua...
#   eosio.token <= eosio.token::transfer        {"from":"eosio","to":"eosio.ram","quantity":"122.0851 HOPEX","memo":"buy ram"}
#   eosio.token <= eosio.token::transfer        {"from":"eosio","to":"eosio.ramfee","quantity":"0.6135 HOPEX","memo":"ram fee"}
#         eosio <= eosio.token::transfer        {"from":"eosio","to":"eosio.ram","quantity":"122.0851 HOPEX","memo":"buy ram"}
#     eosio.ram <= eosio.token::transfer        {"from":"eosio","to":"eosio.ram","quantity":"122.0851 HOPEX","memo":"buy ram"}
#         eosio <= eosio.token::transfer        {"from":"eosio","to":"eosio.ramfee","quantity":"0.6135 HOPEX","memo":"ram fee"}
#  eosio.ramfee <= eosio.token::transfer        {"from":"eosio","to":"eosio.ramfee","quantity":"0.6135 HOPEX","memo":"ram fee"}
#   eosio.token <= eosio.token::transfer        {"from":"eosio","to":"eosio.stake","quantity":"200000000.0000 HOPEX","memo":"stake bandwidth"}
#         eosio <= eosio.token::transfer        {"from":"eosio","to":"eosio.stake","quantity":"200000000.0000 HOPEX","memo":"stake bandwidth"}
#   eosio.stake <= eosio.token::transfer        {"from":"eosio","to":"eosio.stake","quantity":"200000000.0000 HOPEX","memo":"stake bandwidth"}
warning: transaction executed locally, but may not be confirmed by the network yetult         ]



cleos --url http://127.0.0.1:18888 system regproducer accountnum11 EOS5dM8j4m4Dh9zUzAaGy1DxXHC73HNFd9p93B4mVpg9b6TuNX8DK  https://accountnum11.com 



cd ~/biosboot/
mkdir accountnum11
cd accountnum11
cp ~/biosboot/genesis/stop.sh
cp ~/biosboot/genesis/clean.sh



cleos create key --to-console
Private key: 5JR4iPwdTbdeunpTCe4HVmrXRxo2PKFy25wc6bNzWHrN2wnD6Fb
Public key: EOS5To17zgyaRKMYNtFroBi6tbrSFGyUVSw3yztKhHS7vejfSbe3K


cleos wallet import --private-key -n zyy


cleos  --url http://127.0.0.1:18888 system newaccount eosio --transfer accountnum12 EOS5To17zgyaRKMYNtFroBi6tbrSFGyUVSw3yztKhHS7vejfSbe3K --stake-net "100000000.0000 HOPEX" --stake-cpu "100000000.0000 HOPEX" --buy-ram-kbytes 8192
cleos  --url http://127.0.0.1:18888 system regproducer accountnum12 EOS5To17zgyaRKMYNtFroBi6tbrSFGyUVSw3yztKhHS7vejfSbe3K  https://accountnum12.com 