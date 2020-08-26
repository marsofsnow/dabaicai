cd ./build
cmake ..
make -j8
cd ..
/usr/opt/eosio/2.0.7/bin/cleos --url http://127.0.0.1:18012 --wallet-url http://127.0.0.1:16666 set contract zhouhao ./build/learn  -p zhouhao