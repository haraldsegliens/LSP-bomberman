#ifndef MSG_H
#define MSG_H

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

#endif //MSG_H