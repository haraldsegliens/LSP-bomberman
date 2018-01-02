#ifndef COMMS_H
#define COMMS_H

#include <pthread.h>
#include "msg_queue.h"

typedef struct {
    int kill;
    MsgQueue* recvMessages;
    MsgQueue* sendMessages;
    int socket;
    pthread_t threadSend;
    pthread_t threadRecv;
} Connection;

Connection* newConnection(int socket);
Connection* newConnection(char* addr, int port);
void sendConnection(Connection* con, Msg message);
MsgQueue* getReceivedMessages(Connection* con);
void freeConnection(Connection* con);

struct ConListNode_t {
    struct ConListNode_t* next;
    Connection* con;
    char* addr;
    int port;
};

typedef struct {
    struct ConListNode_t* front;
    struct ConListNode_t* back;
} ConList;

typedef struct {
    int kill;
    int socket;
    ConList* connections;
    pthread_t threadAccept;
} Listener;

Listener* createListener(int port);
void sendListener(struct ConListNode_t* conNode, Msg message);
void freeListener(Listener* listener);

#endif //COMMS_H