#!/bin/sh
set -e
jscheme < $1.in > $1.out 2>&1
diff -q $1.out $1.cmp
