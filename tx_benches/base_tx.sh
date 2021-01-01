#!/bin/bash

declare -i kk=1;

SUFFIX="base.exe"

CMD="rm -f out.txt"
echo $CMD
eval $CMD

for b in "$@"
do
  if [ $kk -gt 1 ]
  then 
    CMD="clang++ -g $b.c -O3 -mrtm -DUSE_TX -o $b\_$SUFFIX";
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
