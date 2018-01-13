bin/test_comms: test/test_comms.c bin/comms.o bin/msg_queue.o
	gcc -o bin/test_comms bin/comms.o bin/msg_queue.o test/test_comms.c -std=c90 -g -Wall -Werror -pthread

bin/comms.o: bin comms/comms.c comms/comms.h
	gcc -o bin/comms.o -c comms/comms.c -std=c90 -g -Wall -Werror

bin/msg_queue.o: bin comms/msg_queue.c
	gcc -o bin/msg_queue.o -c comms/msg_queue.c -std=c90 -g -Wall -Werror

bin:
	mkdir bin

check: bin/test_comms
	sudo ./bin/test_comms single
	sudo ./bin/test_comms multipleMsg

clean:
	rm -f bin/test_comms bin/comms.o bin/msg_queue.o