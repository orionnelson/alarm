#!/bin/sh
set -e
#echo $1
readarray inputs < $1.test
rm -f $1.out
cmd="timeout 30s ./My_Alarm "${inputs[@]}" >> $1.out"
#echo $cmd
gnome-terminal -x bash -c "$cmd; exec bash"
#terminal -e command timeout 30s ./My_Alarm "${inputs[@]}" >> $1.out 
echo `cat $1.out`
 # Max time we will wait for a given test
#get the process Id of teh
#kill -15 $! # 15 is the code for SIGTERM or CNTL +C inside of a Program.
 
printf "\n\nRunning Test Case $1 \n Input is :\n  $(<$1.test) \n\n" 
printf "\nLooking For Expected Output :\n  $(<$1.exp)\n"
printf "\nRecived Output :\n $(<$1.out)\n"

# Commented Out until Later Stages
#diff -q $1.exp $1.out
