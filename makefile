all: cc My_Alarm1.c -D_POSIX_PTHREAD_SEMANTICS -lpthread 

TESTS := expr unrecognised

.PHONY: test
test: for test in $(TESTS); do bash test-runner.sh $$test || exit 1; done

