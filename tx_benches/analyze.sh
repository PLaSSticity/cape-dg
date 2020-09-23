#!/bin/bash

declare -i kk=1;

suf=""
op=""

if [ $2 == "cloak" ]
then 
    suf="\_cloak"
    op="-cloak"
fi

for b in "$@"
do
  if [ $kk -gt 2 ]
  then 
    CMD="clang++ -emit-llvm -c -g $b$suf.c -O3 -mrtm -fno-use-cxa-atexit -o $b$suf.bc";
    echo $CMD;
    eval $CMD;
    CMD="/home/ruiz/vcgit/cape-dg/build/tools/llvm-dg-dump $op $b$suf.bc > $b$suf\_ac.ll 2> $b$suf\_ac.err";
    echo $CMD;
    eval $CMD;

    CMD="clang++ -g $b$suf\_ac.ll -O3 -o $b$suf.exe";
    echo $CMD;
    eval $CMD;

    CMD="./$b$suf.exe infile.txt";
    echo $CMD;
    eval $CMD;
    
    if [ $1 != "none" ]
    then
        CMD="rsync -avz --exclude-from ~/Dropbox/rsync.ignore /home/ruiz/git/dg/tx_benches/$b$suf.exe zhang.5944@$1.cse.ohio-state.edu:/home/zhang.5944/sec_examples/";
        echo $CMD;
        eval $CMD;
    fi   
  fi
  let kk++;
done
