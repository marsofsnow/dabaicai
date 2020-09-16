#!/bin/bash
DATADIR="./blockchain"


if [ ! -d $DATADIR ]; then
  mkdir -p $DATADIR;
fi

if [  -f $DATADIR"/nodeos.log" ]; then
  echo "" > $DATADIR"/nodeos.log"
fi

nodeos \
--signature-provider EOS54r5BTHheF3Dzss8mqedFJeHPwL83ha2TgFq8THG3aCoFz4SCG=KEY:5KYoFyDQjaqGtB7xLxta6PYzvdX4rvhudifdaq3DzogonXJPtLY \
--plugin eosio::producer_plugin \
--plugin eosio::producer_api_plugin \
--plugin eosio::chain_plugin \
--plugin eosio::chain_api_plugin \
--plugin eosio::http_plugin \
--plugin eosio::history_api_plugin \
--plugin eosio::history_plugin \
--data-dir $DATADIR"/data" \
--blocks-dir $DATADIR"/blocks" \
--config-dir $DATADIR"/config" \
--producer-name eosio \
--http-server-address 127.0.0.1:18888 \
--p2p-listen-endpoint 127.0.0.1:19010 \
--access-control-allow-origin=* \
--filter-on=* \
--contracts-console \
--http-validate-host=false \
--verbose-http-errors \
--enable-stale-production \
--p2p-peer-address localhost:19011 \
--p2p-peer-address localhost:19012 \
--p2p-peer-address localhost:19013 \
--hard-replay-blockchain \
>> $DATADIR"/nodeos.log" 2>&1 & \
echo $! > $DATADIR"/eosd.pid"

