all:
	cc My_Alarm1.c -D_POSIX_PTHREAD_SEMANTICS -lpthread

TESTS := test/test1 test/test2

.PHONY: test
check:
	for test in $(TESTS); do bash test-alarm.sh $$test || exit 1; done



	
