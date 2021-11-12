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

diff -q $1.exp $1.out
