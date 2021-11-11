#!/bin/sh
set -e
a.out < $1.test > $1.exp 2>&1
diff -q $1.out $1.cmp
