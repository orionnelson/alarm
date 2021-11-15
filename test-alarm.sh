#!/bin/sh
set -e

readarray inputs < $1.test
timeout 30s ./My_Alarm "${inputs[@]}" >> $1.out

printf "`cat $1.out`"

 # Max time we will wait for a given test
#get the process Id of teh
#kill -15 $! # 15 is the code for SIGTERM or CNTL +C inside of a Program. 
output=`cat $1.out`
printf "\n\nRunning Test Case $1 \n Input is :\n  $(<$1.test) \n\n" 
printf "\nLooking For Expected Output :\n  $(<$1.exp)\n"
printf "\nRecived Output :\n $output\n"

# Commented Out until Later Stages
#diff -q $1.exp $1.out
