if [ ! -d build ]; then
  mkdir -p build;
fi
cd ./build
cmake ..
make -j8
cd ..

zz='/usr/opt/eosio/2.0.7/bin/cleos -v --url http://zhongyingying.qicp.io:38000 --wallet-url http://zhongyingying.qicp.io:38001'



${zz}  set contract otcexchange ./build/otcexchange  -p otcexchange

${zz}  push action otcexchange  hi '["zhou"]' -p zhouhao 



${zz} push action otcsystem signup '[ "zhouhao", "", "zhouhao" ]' -p zhouhao
${zz} push action otcsystem signup '[ "otcexchange", "daqwqqq", "zhouhao" ]' -p zhouhao
${zz} push action otcsystem signup '[ "dabaicai", "", "dabaicai" ]' -p dabaicai
${zz} push action otcsystem signup '[ "a1", "a111", "a1111" ]' -p a1
${zz} push action otcsystem signup '[ "a1", "", "a1111" ]' -p a1
${zz} push action otcsystem signup '[ "a2", "", "a2111" ]' -p a2
${zz} push action otcsystem signup '[ "a3", "", "a311111" ]' -p a3
${zz} push push action otcsystem signin '["zhouhao","2222","abcd","abcd"]' -p zhouhao@active
${zz} push push action otcsystem signin '["dabaicai","2222","ab","ad"]' -p dabaicai@active
${zz} push push action otcsystem signin '["a1","2222","ab","ad"]' -p a1@active
${zz} push push action otcsystem signin '["a2","2222","ab","ad"]' -p a2@active
${zz} push push action otcsystem signin '["a3","2222","ab","ad"]' -p a3@active
${zz} set account permission zhouhao active '{"threshold": 1,"keys": [{"key": "EOS66uWg77AUeV7PxW5CejEHen7roZzickkb3RoBgQ1dYMXDEpjrT","weight": 1}], "accounts": [{"permission":{"actor":"otcexchange","permission":"eosio.code"},"weight":1},{"permission":{"actor":"otcsystem","permission":"eosio.code"},"weight":1}]}' -p zhouhao@owner
${zz} set account permission dabaicai active '{"threshold": 1,"keys": [{"key": "EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9","weight": 1}], "accounts": [{"permission":{"actor":"otcexchange","permission":"eosio.code"},"weight":1},{"permission":{"actor":"otcsystem","permission":"eosio.code"},"weight":1}]}' -p dabaicai@owner
${zz} set account permission a1 active '{"threshold": 1,"keys": [{"key": "EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9","weight": 1}], "accounts": [{"permission":{"actor":"otcexchange","permission":"eosio.code"},"weight":1},{"permission":{"actor":"otcsystem","permission":"eosio.code"},"weight":1}]}' -p a1@owner
${zz} set account permission a2 active '{"threshold": 1,"keys": [{"key": "EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9","weight": 1}], "accounts": [{"permission":{"actor":"otcexchange","permission":"eosio.code"},"weight":1},{"permission":{"actor":"otcsystem","permission":"eosio.code"},"weight":1}]}' -p a2@owner
${zz} set account permission a3 active '{"threshold": 1,"keys": [{"key": "EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9","weight": 1}], "accounts": [{"permission":{"actor":"otcexchange","permission":"eosio.code"},"weight":1},{"permission":{"actor":"otcsystem","permission":"eosio.code"},"weight":1}]}' -p a3@owner

${zz}  get table otcexchange otcexchange markets

