#!/bin/bash

declare -i kk=1;

CMD="rm -f out.txt"
echo $CMD
eval $CMD

for b in "$@"
do
    if [ $kk -gt 1 ]
    then 
        # cloak_tx
            CMD="clang++ -emit-llvm -c $b\_cloak.c -mrtm -O3 -DUSE_TX -fno-use-cxa-atexit -o $b\_cloak_tx.bc";
            echo $CMD;
            eval $CMD;

            CMD="../tools/llvm-dg-dump -cloak $b\_cloak_tx.bc > $b\_cloak_ac_tx.ll 2> $b\_cloak_ac_tx.err";
            echo $CMD;
            eval $CMD;

            #CMD="llvm-as $b\_cloak_ac_tx.ll";
            #echo $CMD;
            #eval $CMD;

            CMD="clang++ $b\_cloak_ac_tx.ll -O3 -o $b\_cloak_tx.exe";
            echo $CMD;
            eval $CMD;

            CMD="./$b\_cloak_tx.exe infile.txt out.txt";
            echo $CMD;
            eval $CMD;
            
        for i in {0..3}
        do
            NO_TX=""
            NO_PRELAD=""
            s=$i
            if [ $i == 0 ]
            then
                NO_TX="-DNO_TX"
                NO_PRELAD="-DNO_PRELD"
            elif [ $i == 1 ]
            then
                NO_PRELAD="-DNO_PRELD"
            elif [ $i == 2 ]
            then
                NO_TX="-DNO_TX"
            else
                s=""
            fi   
            
            CMD="clang++ -emit-llvm -c $b.c -mrtm -O3 -DUSE_TX $NO_TX $NO_PRELAD -fno-use-cxa-atexit -o $b\_tx$s.bc";
            echo $CMD;
            eval $CMD;

            CMD="../tools/llvm-dg-dump $b\_tx$s.bc > $b\_ac\_tx$s.ll 2> $b\_ac\_tx$s.err";
            echo $CMD;
            eval $CMD;

            CMD="clang++ $b\_ac\_tx$s.ll -O3 -o $b\_tx$s.exe";
            echo $CMD;
            eval $CMD;

            CMD="./$b\_tx$s.exe infile.txt out.txt";
            echo $CMD;
            eval $CMD;
        done
        
        if [ $1 != "none" ]
        then
            CMD="rsync -avz --exclude-from ~/Dropbox/rsync.ignore /home/ruiz/git/dg/tx_benches/$b*tx*.exe zhang.5944@$1.cse.ohio-state.edu:/home/zhang.5944/sec_examples/";
            echo $CMD;
            eval $CMD;
        fi
    fi # kk == 2
let kk++;  
done
