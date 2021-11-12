#!/bin/sh
set -e
echo "$(<$1.test)"
./a.out < $1.test > $1.out 2>&1

for i in $n_procs; do
    ./procs[${i}] &
    pids[${i}]=$!
done

# wait for all pids
for pid in ${pids[*]}; do
    wait $pid
done
echo "Running Test Case $1 \n\n"
echo "Looking For Expected Output :\n  $(<$1.exp)"
echo "Recived Output :\n $(<$1.out)"


diff -q $1.exp $1.out
