#!/bin/bash

declare -i kk=1;

for b in "$@"
do
  if [ $kk -gt 1 ]
  then 
    CMD="clang++ -emit-llvm -c -g $b.c -O3 -mrtm -fno-use-cxa-atexit -o $b.bc";
    echo $CMD;
    eval $CMD;

    CMD="../tools/llvm-dg-dump -cloak $b.bc > $b.s 2> $b.err";
    echo $CMD;
    eval $CMD;

    CMD="llvm-as $b.s";
    echo $CMD;
    eval $CMD;

    CMD="clang++ -g $b.s.bc -O3 -o $b.exe";
    echo $CMD;
    eval $CMD;

    CMD="./$b.exe infile.txt";
    echo $CMD;
    eval $CMD;
    
    if [ $1 != "none" ]
    then
        CMD="rsync -avz --exclude-from ~/Dropbox/rsync.ignore /home/ruiz/git/dg/tx_benches/$b.exe zhang.5944@$1.cse.ohio-state.edu:/home/zhang.5944/sec_examples/";
        echo $CMD;
        eval $CMD;
    fi   
  fi
  let kk++;
done
