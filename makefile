all:
	cc My_Alarm1.c -D_POSIX_PTHREAD_SEMANTICS -lpthread

TESTS := test1

.PHONY: test
check:
	for test in $(TESTS); do bash test-alarm.sh $$test || exit 1; done



	
