#include "comms.h"
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_MESSAGE 10000

void* handleRecv(void* params) {
    Connection* con = (Connection*)params;
    char buffer[MAX_MESSAGE];
    while(!con->kill) {

        int len = recv(con->socket, buffer, MAX_MESSAGE, 0);
        if (len < 0) {
            printf("ignored recv error packet\n");
            continue;
        }

        printf("received %d bytes",len);

        Msg message;
        message.type = handleParams.type;
        message.buffer = malloc(len+1);
        strncpy(message.buffer,buffer,len);
        message.buffer[len] = '\0';
        enqueueMsgQueue(con->recvMessages,message);
    }

    return NULL;
}

void* handleSend(void* params) {
    Connection* con = (Connection*)params;
    while(!con->kill) {
        Msg message = dequeueMsgQueue(con->sendMessages);
        if(message.buffer_length == 0) {
            continue;
        }
        send(con->socket,
             message.buffer, 
             message.buffer_length, 
             0
        );
    }
    return NULL;
}

Connection* newConnection(int socket) {
    Connection* con = malloc(sizeof(Connection));
    con->kill = 0;
    con->recvMessages = newMsgQueue();
    con->sendMessages = newMsgQueue();
    con->socket = socket;
    pthread_create(&con->threadRecv, NULL, handleRecv, (void *) con);
    pthread_create(&con->threadSend, NULL, handleSend, (void *) con);
    return con;
}

Connection* newConnection(char* addr, int port) {
    Connection* con = malloc(sizeof(Connection));
    con->kill = 0;
    con->recvMessages = newMsgQueue();
    con->sendMessages = newMsgQueue();
    con->socket = socket(AF_INET , SOCK_STREAM , 0);
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(addr);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
        perror("connect failed. Error");
        return NULL;
    }
    pthread_create(&con->threadRecv, NULL, handleRecv, (void *) con);
    pthread_create(&con->threadSend, NULL, handleSend, (void *) con);
    return con;
}

void sendConnection(Connection* con, Msg message) {
    enqueueMsgQueue(con->sendMessages,message);
}

MsgQueue* getReceivedMessages(Connection* con) {
    return dequeueAllMsgQueue(con->recvMessages);
}

void freeConnection(Connection* con) {
    con->kill = 1;
    pthread_join(con->threadSend, NULL);
    pthread_join(con->threadRecv, NULL);
    freeMsgQueue(recvMessages);
    freeMsgQueue(sendMessages);
    close(socket);
    free(con);
}

ConList* newConList() {
    ConList* conList = malloc(sizeof(ConList));
    conList->front = NULL;
    conList->back = NULL;
    return conList;
}

void addConListNode(ConList* list, struct ConListNode_t* node) {
    if(list->front == NULL) {
        list->front = node;
        list->back = node;    
    } else {
        list->back->next = node;
        list->back = node;
    }
}

struct ConListNode_t* newConListNode(Connection* con, char* addr, int port) {
    struct ConListNode_t* node = malloc(sizeof(struct ConListNode_t));
    node->next = NULL;
    node->con = con;
    node->addr = strcpy(addr);
    node->port = port;
    return node;
}

void* handleAccept(void* params) {
    Listener* listener = (Listener*)params;
    while(!listener->kill) {
        struct sockaddr_in addr;
        socklen_t addrSize = sizeof(addr);
        int clientFd = accept(tcpFd, (struct sockaddr *) &addr, &addrSize);
        if (clientFd == -1) {
            printf("ignored accept error\n");
            continue;
        }

        struct ConListNode_t* node = newConListNode(
            newConnection(clientFd),
            inet_ntoa(addr.sin_addr),
            ntohs(addr.sin_port)
        );
        addConListNode(listener->connections, node);

        printf("connected with %s:%d",node->addr,node->port);
    }

    return NULL;
}

Listener* createListener(int port, int connections_count) {
    Listener* listener = malloc(sizeof(Listener));
    listener->kill = 0;
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

void sendListener(struct ConListNode_t* conNode, Msg message) {
    sendConnection(conNode->con,message);        
}

void freeListener(Listener* listener) {
    listener->kill = 1;
    pthread_join(listener->threadAccept, NULL);
    struct ConListNode_t* tmp;
    struct ConListNode_t* i_node = list->front;
    while(i_node != NULL) { 
        freeConnection(i_node->con);
        tmp = i_node;
        i_node = i_node->next;
        free(tmp);
    }
    free(listener->connections);
    close(listener->socket);
    free(listener);
}