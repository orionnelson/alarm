all: ; cc My_Alarm1.c -D_POSIX_PTHREAD_SEMANTICS -lpthread

check := expr unrecognised

.PHONY: test
test: ; for test in $(check); do bash test-runner.sh $$test || exit 1; done
