#ifndef COMMS_H
#define COMMS_H

#include <pthread.h>

typedef struct {
    char* buffer;
    long buffer_length;
} Msg;

struct MsgQueueNode_t {
    struct MsgQueueNode_t* next;
    Msg message;
};

typedef struct {
    pthread_mutex_t lock;
    struct MsgQueueNode_t* front;
    struct MsgQueueNode_t* back;
} MsgQueue;

MsgQueue* newMsgQueue();
void enqueueMsgQueue(MsgQueue* queue, Msg message);
Msg dequeueMsgQueue(MsgQueue* queue);
MsgQueue* dequeueAllMsgQueue(MsgQueue* queue);
void freeMsgQueue(MsgQueue* queue);

struct ConListNode_t;

typedef struct {
    struct ConListNode_t* front;
    struct ConListNode_t* back;
    pthread_mutex_t lock;
} ConList;

typedef struct {
    ConList* conList;
    MsgQueue* recvMessages;
    MsgQueue* sendMessages;
    int socket;
    pthread_t threadSend;
    pthread_t threadRecv;
    char* peerAddr;
    int peerPort;
} Connection;

Connection* newServerConnection(int socket, char* addr, int port, ConList* conList);
Connection* newClientConnection(char* addr, int port);
void sendConnection(Connection* con, Msg message);
MsgQueue* getReceivedMessages(Connection* con);
void freeConnection(Connection* con);

struct ConListNode_t {
    struct ConListNode_t* next;
    Connection* con;
} ConListNode;

void addConList(ConList* list, struct ConListNode_t* node);
void removeConList(ConList* list, Connection* con);
void freeConList(ConList* list);

typedef struct {
    int socket;
    ConList* connections;
    pthread_t threadAccept;
} Listener;

Listener* createListener(int port, int connections_count);
void freeListener(Listener* listener);

#endif