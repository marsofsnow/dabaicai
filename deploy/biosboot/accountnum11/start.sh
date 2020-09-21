#!/bin/bash
DATADIR="./blockchain"
CURDIRNAME=${PWD##*/}

if [ ! -d $DATADIR ]; then
  mkdir -p $DATADIR;
fi

if [  -f $DATADIR"/nodeos.log" ]; then
  echo "" > $DATADIR"/nodeos.log"
fi

nodeos \
--signature-provider EOS5BGsTksjoPuGrpTQGNDAD3wTvUoD8tioo4KgRjccfwEpF7HZzw=KEY:5KXzEgnjmsCTD8c6wPg3M24vF4SCNnJJQcpuperu7mS6hETrLv1 \
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
--http-server-address 127.0.0.1:18011 \
--p2p-listen-endpoint 127.0.0.1:19011 \
--access-control-allow-origin=* \
--contracts-console \
--http-validate-host=false \
--verbose-http-errors \
--enable-stale-production \
--filter-on=* \
--p2p-peer-address localhost:19010 \
--p2p-peer-address localhost:19012 \
--p2p-peer-address localhost:19013 \
>> $DATADIR"/nodeos.log" 2>&1 & \
echo $! > $DATADIR"/eosd.pid"
