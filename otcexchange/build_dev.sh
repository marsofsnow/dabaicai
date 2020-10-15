if [ ! -d build ]; then
  mkdir -p build;
fi
cd ./build
cmake ..
make -j8
cd ..

echo "1"
zz='/usr/opt/eosio/2.0.7/bin/cleos -v --url http://zhongyingying.qicp.io:38000 --wallet-url http://zhongyingying.qicp.io:38001'

${zz}  set contract otcexchange ./build/otcexchange  -p otcexchange
echo "2"
${zz}  push action otcexchange  hi '["zhou"]' -p zhouhao 





