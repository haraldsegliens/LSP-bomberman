#include "comms.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_MESSAGE 10000

/*void printEscaped(char* string, int len) {
    int i = 0;
    for(i = 0; i < len; i++) {
        if(string[i] == '\0') {
            printf("\\0");
        } else if(string[i] == '\n') {
            printf("\\n");
        } else {
            printf("%c",string[i]);
        }
    }
}*/

void* handleRecv(void* params) {
    Connection* con = (Connection*)params;
    char buffer[MAX_MESSAGE];
    while(1) {
        int len = recv(con->socket, buffer, MAX_MESSAGE, 0);
        if (len < 0) {
            /*printf("ignored recv error packet\n");*/
            continue;
        } else if(len == 0) {
            freeConnection(con);
            return NULL;
        }

        /*printf("received %d bytes from %s:%d ", len, con->peerAddr, con->peerPort);
        printEscaped(buffer,len);
        printf("\n");*/

        Msg message;
        message.buffer = malloc(len+1);
        memcpy(message.buffer,buffer,len);
        message.buffer[len] = '\0';
        message.buffer_length = len;
        enqueueMsgQueue(con->recvMessages,message);
    }
    return NULL;
}

void* handleSend(void* params) {
    Connection* con = (Connection*)params;
    while(1) {
        Msg message = dequeueMsgQueue(con->sendMessages);
        if(message.buffer_length == 0) {
            continue;
        }
        send(con->socket,message.buffer, message.buffer_length,0);
        /*printf("sent %ld bytes from %s:%d ", message.buffer_length, con->peerAddr, con->peerPort);
        printEscaped(message.buffer,message.buffer_length);
        printf("\n");*/
        free(message.buffer);
    }
    return NULL;
}

Connection* newServerConnection(int socket,char* addr, int port,ConList* conList) {
    Connection* con = malloc(sizeof(Connection));
    con->conList = conList;
    con->recvMessages = newMsgQueue();
    con->sendMessages = newMsgQueue();
    con->socket = socket;
    con->peerAddr = addr;
    con->peerPort = port;
    pthread_create(&con->threadRecv, NULL, handleRecv, (void *) con);
    pthread_create(&con->threadSend, NULL, handleSend, (void *) con);
    /*printf("%s:%d connected\n",con->peerAddr,con->peerPort);*/
    return con;
}

Connection* newClientConnection(char* addr, int port) {
    Connection* con = malloc(sizeof(Connection));
    con->conList = NULL;
    con->peerAddr = addr;
    con->peerPort = port;
    con->recvMessages = newMsgQueue();
    con->sendMessages = newMsgQueue();
    con->socket = socket(AF_INET , SOCK_STREAM , 0);
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(addr);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (connect(con->socket, (struct sockaddr *)&server , sizeof(server)) < 0) {
        perror("connect failed. Error");
        return NULL;
    }
    pthread_create(&con->threadRecv, NULL, handleRecv, (void *) con);
    pthread_create(&con->threadSend, NULL, handleSend, (void *) con);
    /*printf("connected to %s:%d\n",con->peerAddr,con->peerPort);*/
    return con;
}

void sendConnection(Connection* con, Msg message) {
    /*make a copy of the message*/
    char* temp = (char*)malloc(message.buffer_length);
    memcpy(temp,message.buffer,message.buffer_length);
    message.buffer = temp;
    if(con == NULL) {
        perror("sendConnection failed: con null");
    }
    enqueueMsgQueue(con->sendMessages,message);
}

MsgQueue* getReceivedMessages(Connection* con) {
    return dequeueAllMsgQueue(con->recvMessages);
}

void freeConnection(Connection* con) {
    pthread_cancel(con->threadSend);
    pthread_cancel(con->threadRecv);
    if(con->conList != NULL) {
        removeConList(con->conList,con);
    }
    freeMsgQueue(con->recvMessages);
    freeMsgQueue(con->sendMessages);
    shutdown(con->socket,2);
    /*printf("disconnected from %s:%d\n", con->peerAddr, con->peerPort);*/
    free(con);
}

struct ConListNode_t* newConListNode(Connection* con) {
    struct ConListNode_t* node = malloc(sizeof(struct ConListNode_t));
    node->next = NULL;
    node->con = con;
    return node;
}

ConList* newConList() {
    ConList* conList = malloc(sizeof(ConList));
    conList->front = NULL;
    conList->back = NULL;
    pthread_mutex_init(&conList->lock, NULL);
    return conList;
}

void addConList(ConList* list, struct ConListNode_t* node) {
    pthread_mutex_lock(&list->lock);
    if(list->front == NULL) {
        list->front = node;
        list->back = node;    
    } else {
        list->back->next = node;
        list->back = node;
    }
    pthread_mutex_unlock(&list->lock);
}

void removeConList(ConList* list, Connection* con) {
    pthread_mutex_lock(&list->lock);
    if(list->front == list->back && list->front->con == con) {
        free(list->front);
        list->front = NULL;
        list->back = NULL;
    } else if(list->front->con == con) {
        free(list->front);
        list->front = list->front->next;
    } else {
        struct ConListNode_t* i_prev = list->front;
        struct ConListNode_t* i_node = list->front->next;
        while(i_node != NULL) { 
            if(i_node->con == con) {
                i_prev->next = i_node->next;
                free(i_node);
                break;
            }
            i_prev = i_node;
            i_node = i_node->next;
        }
    }
    pthread_mutex_unlock(&list->lock);
}

void freeConList(ConList* list) {
    struct ConListNode_t* tmp;
    pthread_mutex_lock(&list->lock);
    struct ConListNode_t* i_node = list->front;
    while(i_node != NULL) { 
        freeConnection(i_node->con);
        tmp = i_node;
        i_node = i_node->next;
        free(tmp);
    }
    pthread_mutex_destroy(&list->lock);
    free(list);    
}

void* handleAccept(void* params) {
    Listener* listener = (Listener*)params;
    while(1) {
        struct sockaddr_in addr;
        socklen_t addrSize = sizeof(addr);
        int clientFd = accept(listener->socket, (struct sockaddr *) &addr, &addrSize);
        if (clientFd == -1) {
            continue;
        }

        struct ConListNode_t* node = newConListNode(newServerConnection(
            clientFd,
            inet_ntoa(addr.sin_addr),
            ntohs(addr.sin_port),
            listener->connections
        ));
        addConList(listener->connections, node);
    }

    return NULL;
}

Listener* createListener(int port, int connections_count) {
    Listener* listener = malloc(sizeof(Listener));
    listener->connections = newConList();
    listener->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listener->socket < 0) {
        perror("tcp socket");
        return NULL;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (bind(listener->socket, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) == -1) {
        perror("tcp bind");
        return NULL;
    }

    if (listen(listener->socket, connections_count) == -1) {
        perror("listen");
        return NULL;
    }
    pthread_create(&listener->threadAccept, NULL, handleAccept, (void *) listener);
    return listener;
}

void freeListener(Listener* listener) {
    pthread_cancel(listener->threadAccept);
    freeConList(listener->connections);
    shutdown(listener->socket,2);
    free(listener);
}