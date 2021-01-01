#!/bin/bash

declare -i kk=1;

TX=""
SUFFIX="cloak.exe"

if [ $2 == "tx" ]
then
    TX="-DUSE_TX"
    SUFFIX="cloak_tx.exe"
    CMD="rm -f out.txt"
    echo $CMD
    eval $CMD
fi

for b in "$@"
do
  if [ $kk -gt 2 ]
  then 
    CMD="clang++ -g $b\_cloak.c $TX -O3 -mrtm -o $b\_$SUFFIX";
    echo $CMD;
    eval $CMD;

    CMD="./$b\_$SUFFIX infile.txt out.txt";
    echo $CMD;
    eval $CMD;
    
    if [ $1 != "none" ]
    then
        CMD="rsync -avz --exclude-from ~/Dropbox/rsync.ignore /home/ruiz/git/dg/tx_benches/$b\_$SUFFIX zhang.5944@$1.cse.ohio-state.edu:/home/zhang.5944/sec_examples/";
        echo $CMD;
        eval $CMD;
    fi   
  fi
  let kk++;
done
