#!/bin/bash
DATADIR=${PWD}"/wallet"

if [ -f $DATADIR"/keosd.pid" ]; then
pid=`cat $DATADIR"/keosd.pid"`
echo $pid
kill -SIGTERM $pid
rm -r $DATADIR"/keosd.pid"
echo -ne "Stoping Node"
while true; do
[ ! -d "/proc/$pid/fd" ] && break
echo -ne "."
sleep 1
done
echo -ne "\rNode Stopped. \n"
fi