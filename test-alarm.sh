#!/bin/sh
set -e
readarray inputs < $1.test
rm -f $1.out
#timeout -s SIGINT 30s ./My_Alarm "${inputs[@]}" >> $1.out # Kills current shell fml
timeout -s SIGINT 30s ./My_Alarm "${inputs[@]}" >> $1.out

# I HATE SHELL
