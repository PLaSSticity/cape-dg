#!/bin/bash 

for i in {0..3}
do
  s=""
  if [ $i -lt 3 ]
  then 
    s=$i
  fi
    
  CMD="llvm-as aes/aes\_ac\_tx$s.ll";
  echo $CMD;
  eval $CMD;

  CMD="clang++ -O3 aes/aes\_ac\_tx$s.bc -o aes/aes\_tx$s.exe";
  echo $CMD;
  eval $CMD;
done

if [ $1 != "none" ]
  then
    CMD="rsync -avz --exclude-from ~/Dropbox/rsync.ignore /home/ruiz/git/dg/tx_benches/aes/aes\_tx*.exe zhang.5944@$1.cse.ohio-state.edu:/home/zhang.5944/sec_examples/";
    echo $CMD;
    eval $CMD;
fi   
