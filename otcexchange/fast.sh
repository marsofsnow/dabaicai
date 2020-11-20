if [  -f ./build.log ]; then
  echo "" > ./build.log
fi
./build_dev.sh  >> ./build.log 2>&1 & 

