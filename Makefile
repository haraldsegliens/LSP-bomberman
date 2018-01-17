CCX = g++
CC = gcc
CXXFLAGS = -std=c++11 -g -Wall -Werror
CFLAGS = -std=c90 -g -Wall -Werror
SERVER_OBJ = bin/server/cgamerules.o bin/server/gamerules_shared.o \
			bin/server/cgamerules_messages.o bin/server/cplayer.o bin/server/cworld.o \
			bin/server/cvolatile_entities_manager.o \
			bin/server/volatile_entities_manager_shared.o \
			bin/server/cdynamite.o
CLIENT_OBJ = bin/client/c_gamerules.o bin/client/gamerules_shared.o \
			bin/client/c_gamerules_messages.o bin/client/c_player.o \
			bin/client/c_world.o bin/client/c_volatile_entities_manager.o \
			bin/client/volatile_entities_manager_shared.o \
			bin/client/c_dynamite.o bin/client/c_screen.o

all: server_launcher client_launcher

server_launcher: bin/server/ $(SERVER_OBJ) bin/comms.o bin/msg_queue.o server_launcher.cpp 
	$(CCX) -o server_launcher $(SERVER_OBJ) bin/comms.o bin/msg_queue.o server_launcher.cpp -pthread -lsfml-graphics -lsfml-window -lsfml-system $(CXXFLAGS_SERVER)

client_launcher: bin/client/ $(CLIENT_OBJ) bin/comms.o bin/msg_queue.o client_launcher.cpp
	$(CCX) -o client_launcher $(CLIENT_OBJ) bin/comms.o bin/msg_queue.o client_launcher.cpp -pthread -lsfml-graphics -lsfml-window -lsfml-system $(CXXFLAGS_CLIENT)

#serverside
CXXFLAGS_SERVER = $(CXXFLAGS) -DSERVER
bin/server/cgamerules.o: server/cgamerules.cpp
	$(CCX) -o bin/server/cgamerules.o -c server/cgamerules.cpp $(CXXFLAGS_SERVER)

bin/server/gamerules_shared.o: shared/gamerules.cpp
	$(CCX) -o bin/server/gamerules_shared.o -c shared/gamerules.cpp $(CXXFLAGS_SERVER)

bin/server/cgamerules_messages.o: server/cgamerules_messages.cpp
	$(CCX) -o bin/server/cgamerules_messages.o -c server/cgamerules_messages.cpp $(CXXFLAGS_SERVER)

bin/server/cplayer.o: server/cplayer.cpp
	$(CCX) -o bin/server/cplayer.o -c server/cplayer.cpp $(CXXFLAGS_SERVER)

bin/server/cworld.o: server/cworld.cpp
	$(CCX) -o bin/server/cworld.o -c server/cworld.cpp $(CXXFLAGS_SERVER)

bin/server/cvolatile_entities_manager.o: server/cvolatile_entities_manager.cpp
	$(CCX) -o bin/server/cvolatile_entities_manager.o -c server/cvolatile_entities_manager.cpp $(CXXFLAGS_SERVER)

bin/server/volatile_entities_manager_shared.o: shared/volatile_entities_manager.cpp
	$(CCX) -o bin/server/volatile_entities_manager_shared.o -c shared/volatile_entities_manager.cpp $(CXXFLAGS_SERVER)

bin/server/cdynamite.o: server/cdynamite.cpp
	$(CCX) -o bin/server/cdynamite.o -c server/cdynamite.cpp $(CXXFLAGS_SERVER)

bin/server/: bin
	mkdir -p bin/server

#clientside
CXXFLAGS_CLIENT = $(CXXFLAGS) -DCLIENT
bin/client/c_gamerules.o: client/c_gamerules.cpp
	$(CCX) -o bin/client/c_gamerules.o -c client/c_gamerules.cpp $(CXXFLAGS_CLIENT)

bin/client/gamerules_shared.o: shared/gamerules.cpp
	$(CCX) -o bin/client/gamerules_shared.o -c shared/gamerules.cpp $(CXXFLAGS_CLIENT)

bin/client/c_gamerules_messages.o: client/c_gamerules_messages.cpp
	$(CCX) -o bin/client/c_gamerules_messages.o -c client/c_gamerules_messages.cpp $(CXXFLAGS_CLIENT)

bin/client/c_player.o: client/c_player.cpp
	$(CCX) -o bin/client/c_player.o -c client/c_player.cpp $(CXXFLAGS_CLIENT)

bin/client/c_world.o: client/c_world.cpp
	$(CCX) -o bin/client/c_world.o -c client/c_world.cpp $(CXXFLAGS_CLIENT)

bin/client/c_volatile_entities_manager.o: client/c_volatile_entities_manager.cpp
	$(CCX) -o bin/client/c_volatile_entities_manager.o -c client/c_volatile_entities_manager.cpp  $(CXXFLAGS_CLIENT)

bin/client/volatile_entities_manager_shared.o: shared/volatile_entities_manager.cpp
	$(CCX) -o bin/client/volatile_entities_manager_shared.o -c shared/volatile_entities_manager.cpp $(CXXFLAGS_CLIENT)

bin/client/c_dynamite.o: client/c_dynamite.cpp
	$(CCX) -o bin/client/c_dynamite.o -c client/c_dynamite.cpp $(CXXFLAGS_CLIENT)

bin/client/c_screen.o: client/c_screen.cpp
	$(CCX) -o bin/client/c_screen.o -c client/c_screen.cpp $(CXXFLAGS_CLIENT)

bin/client/: bin
	mkdir -p bin/client

bin/test_comms2: bin test/test_comms2.cpp bin/comms.o bin/msg_queue.o
	$(CCX) -o bin/test_comms2 bin/comms.o bin/msg_queue.o test/test_comms2.cpp $(CXXFLAGS) -pthread

#C communication module
bin/test_comms: bin test/test_comms.c bin/comms.o bin/msg_queue.o
	$(CC) -o bin/test_comms bin/comms.o bin/msg_queue.o test/test_comms.c $(CFLAGS) -pthread

bin/comms.o: bin comms/comms.c comms/comms.h
	$(CC) -o bin/comms.o -c comms/comms.c $(CFLAGS)

bin/msg_queue.o: bin comms/msg_queue.c
	$(CC) -o bin/msg_queue.o -c comms/msg_queue.c $(CFLAGS)

bin:
	mkdir bin

check: bin/test_comms
	sudo ./bin/test_comms single
	sudo ./bin/test_comms multipleMsg

clean:
	rm -rf bin server_launcher client_launcher