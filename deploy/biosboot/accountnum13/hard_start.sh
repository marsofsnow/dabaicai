#!/bin/bash
DATADIR="./blockchain"
CURDIRNAME=${PWD##*/}

if [ ! -d $DATADIR ]; then
  mkdir -p $DATADIR;
fi

nodeos \
--signature-provider EOS5kRFTVqLMk8Cc5oTzdriN2hnat87AsmmujgYZg7rWGG1MJkJFW=KEY:5JFFhaWdckFjXqDfUw2hGj3Ht4W1KX9BeX22MfJ8EmG23QcV5Es \
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
--producer-name $CURDIRNAME \
--http-server-address 127.0.0.1:18013 \
--p2p-listen-endpoint 127.0.0.1:19013 \
--access-control-allow-origin=* \
--contracts-console \
--http-validate-host=false \
--verbose-http-errors \
--enable-stale-production \
--filter-on=* \
--p2p-peer-address localhost:19010 \
--p2p-peer-address localhost:19011 \
--p2p-peer-address localhost:19012 \
--hard-replay-blockchain \
>> $DATADIR"/nodeos.log" 2>&1 & \
echo $! > $DATADIR"/eosd.pid"
