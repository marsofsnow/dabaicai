cd ./build
cmake ..
make -j8
cd ..
/usr/opt/eosio/2.0.7/bin/cleos --url http://127.0.0.1:18011 --wallet-url http://127.0.0.1:16666 set contract taker ./build/taker  -p taker