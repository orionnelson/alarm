#!/bin/sh
set -e
echo $1
sleep 35

printf "\n\nRunning Test Case $1 \n Input is :\n  $(<$1.test) \n\n" 
printf "\nLooking For Expected Output :\n  $(<$1.exp)\n"
printf "\nRecived Output :\n $(<$1.out)\n"

# Commented Out until Later Stages
#diff -q $1.exp $1.out
