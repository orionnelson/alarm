all:
	cc new_alarm_mutex.c -D_POSIX_PTHREAD_SEMANTICS -lpthread

TESTS := test1 test2 test3 test4

.PHONY: test
check:
	for test in $(TESTS); do bash test-alarm.sh $$test || exit 1; done



	
