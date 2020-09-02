cd ./build
cmake ..
make -j8
cd ..

cc='/usr/opt/eosio/2.0.7/bin/cleos -v --url http://127.0.0.1:18011 --wallet-url http://127.0.0.1:16666'
echo ${cc}
${cc}  set contract otcsetting ./build/otcsetting  -p otcsetting
${cc}  push action otcsetting  hi '["zhou"]' -p zhouhao 