all:
	cc new_alarm_mutex.c -D_POSIX_PTHREAD_SEMANTICS -lpthread

TESTS := expr unrecognised

.PHONY: test
test:
	for test in $(TESTS); do bash test-alarm.sh $$test || exit 1; done



	
