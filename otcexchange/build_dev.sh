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


${zz}   get table otcexchange otcexchange markets

${zz}  push push action otcsystem login '["zhouhao","2222","abcd","abcd"]' -p zhouhao@active
${zz}  push push action otcsystem login '["dabaicai","2222","ab","ad"]' -p dabaicai@active
${zz}  set account permission zhouhao active '{"threshold": 1,"keys": [{"key": "EOS66uWg77AUeV7PxW5CejEHen7roZzickkb3RoBgQ1dYMXDEpjrT","weight": 1}], "accounts": [{"permission":{"actor":"otcexchange","permission":"eosio.code"},"weight":1},{"permission":{"actor":"otcsystem","permission":"eosio.code"},"weight":1}]}' -p zhouhao@owner
${zz}  set account permission dabaicai active '{"threshold": 1,"keys": [{"key": "EOS6JLFKjgXaVWjJihTufhTxB8PF6hm3e4usmJhSXP1pfWcQWapX9","weight": 1}], "accounts": [{"permission":{"actor":"otcexchange","permission":"eosio.code"},"weight":1},{"permission":{"actor":"otcsystem","permission":"eosio.code"},"weight":1}]}' -p dabaicai@owner

