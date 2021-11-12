#!/bin/sh
set -e
./a.out < $1.test > $1.out 2>&1

for i in $n_procs; do
    ./procs[${i}] &
    pids[${i}]=$!
done

# wait for all pids
for pid in ${pids[*]}; do
    wait $pid
done
printf "Running Test Case $1 \n\n"
printf "Looking For Expected Output :\n  $(<$1.exp)"
printf "Recived Output :\n $(<$1.out)"

# Commented Out until Later Stages
#diff -q $1.exp $1.out
