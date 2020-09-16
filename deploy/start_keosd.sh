
WALLET_PATH="/home/zh/eosio/eos/deploy/wallet"
/usr/opt/eosio/2.0.7/bin/keosd  --config-dir ${WALLET_PATH} \
--unix-socket-path ${WALLET_PATH}/keosd.sock  \
--http-server-address 127.0.0.1:16666 \
--unlock-timeout 999999999  \
--wallet-dir ${WALLET_PATH} \
>> ${WALLET_PATH}"/keosd.log" 2>&1 & \
echo $! > ${WALLET_PATH}"/keosd.pid" 


#cc wallet open -n hopex
#echo  -e "PW5JtDturSx1Gw4P5Aax9o1RRxVRSwoH5FT6YeT7z7QN2jjL5uPk6" |  cc wallet  unlock -n hopex
#cc wallet list


