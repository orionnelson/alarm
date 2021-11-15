#!/bin/sh
set -e
input=`cat $1.test`
IFS='\n' read -r -a inputs <<< "$input"

for element in "${inputs[@]}"
do
    printf "$element"
done


./a.out $inputs >> $1.out 2>&1 
#p=`echo $!`
sleep 40 # Max time we will wait for a given test
#get the process Id of teh
#kill -15 $p # 15 is the code for SIGTERM or CNTL +C inside of a Program. 


printf "\n\nRunning Test Case $1 \n Input is :\n  $(<$1.test) \n\n" 
printf "\nLooking For Expected Output :\n  $(<$1.exp)\n"
printf "\nRecived Output :\n $(<$1.out)\n"

# Commented Out until Later Stages
#diff -q $1.exp $1.out
