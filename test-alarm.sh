#!/bin/sh
set -e

readarray inputs < $1.test
for element in "${inputs[@]}"
do
    printf "$element"
done


sleep 30 | ./My_Alarm "5 chains" "4 cattle" "2 milk" >> $1.out & ps -fj; echo "$!"


 # Max time we will wait for a given test
#get the process Id of teh
#kill -15 $! # 15 is the code for SIGTERM or CNTL +C inside of a Program. 
output=`cat $1.out`
printf "\n\nRunning Test Case $1 \n Input is :\n  $(<$1.test) \n\n" 
printf "\nLooking For Expected Output :\n  $(<$1.exp)\n"
printf "\nRecived Output :\n $output\n"

# Commented Out until Later Stages
#diff -q $1.exp $1.out
