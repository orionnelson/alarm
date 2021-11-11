all:
	cc new_alarm_mutex.c -D_POSIX_PTHREAD_SEMANTICS -lpthread

TESTS := test1

.PHONY: test
check:
	for test in $(TESTS); do bash test-alarm.sh $$test || exit 1; done



	
