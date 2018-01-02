#include "msg_queue.h"

#include <stdlib.h>

struct MsgQueueNode_t* newMsgQueueNode_t(Msg message) {
    struct MsgQueueNode_t* node = malloc(sizeof(MsgQueueNode_t));
    node->next = NULL;
    node->message = message;
    return node;
}

MsgQueue* newMsgQueue() {
    MsgQueue* msgQueue = malloc(sizeof(MsgQueue));
    pthread_mutex_init(&msgQueue->lock, NULL)
    msgQueue->front = NULL;
    msgQueue->back = NULL;
    return msgQueue;
}

void enqueueMsgQueue(MsgQueue* queue, Msg message) {
    pthread_mutex_lock(&queue->lock);
    MsgQueueNode_t* node = newMsgQueueNode_t(message);
    node.next = NULL;
    if(queue->front != NULL) {
        queue->back->next = node;
        queue->back = node;
    } else {
        queue->front = node;
        queue->back = node;
    }
    pthread_mutex_unlock(&queue->lock);
}

Msg dequeueMsgQueue(MsgQueue* queue) {
    Msg result;
    pthread_mutex_lock(&queue->lock);
    if(queue->front == NULL) {
        result.buffer_length = 0;
    } else if(queue->front == queue->back) {
        result = queue->front->message;
        free(queue->front);
        queue->front = NULL;
        queue->back = NULL;
    } else {
        result = queue->front->message;
        MsgQueueNode_t* temp = queue->front
        queue->front = queue->front->next;
        free(temp);
    }
    pthread_mutex_unlock(&queue->lock);
    return result;
}

MsgQueue* dequeueAllMsgQueue(MsgQueue* queue) {
    MsgQueue* result = newMsgQueue();
    pthread_mutex_lock(&queue->lock);
    result->front = queue->front;
    result->back = queue->back;
    queue->front = NULL;
    queue->back = NULL;
    pthread_mutex_unlock(&queue->lock);
    return result;
}

void freeMsgQueue(MsgQueue* queue) {
    while(queue->front != NULL) {
        dequeue(queue);
    }
    pthread_mutex_destroy(&queue->lock);
    free(queue);
}