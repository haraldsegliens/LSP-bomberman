test_comms: test/test_comms.c bin/comms.o
	gcc -o test_comms bin/comms.o test/test_comms.c -std=c90 -g -Wall -Werror

bin/comms.o: bin comms/comms.c comms/comms.h
	gcc -o bin/comms.o -c comms/comms.c -std=c90 -g -Wall -Werror

bin:
	mkdir bin 