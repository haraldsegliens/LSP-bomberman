#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif /* __STDC_VERSION__ */

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "../comms/comms.h"

#define PORT 9123
#define CALCULATION_DELAY_NSECS 1000000

typedef struct timespec timespec_t;

timespec_t wait_start_time;
timespec_t last_calculation_time;

#define WAITFORCOND(COND,WAIT_TIME,ERROR_TEXT,UPDATE_COND) \
wait_start_time = getCurrentTime();\
last_calculation_time = wait_start_time;\
while(!(COND)) {\
    while(diff(last_calculation_time,getCurrentTime()).tv_nsec < CALCULATION_DELAY_NSECS) {}\
    last_calculation_time = getCurrentTime();\
    if(diff(wait_start_time,getCurrentTime()).tv_sec >= WAIT_TIME) {\
        perror(ERROR_TEXT);\
        return;\
    }\
    UPDATE_COND\
}

int countConnections(Listener* listener) {
    int result = 0;
    pthread_mutex_lock(&listener->connections->lock);
    struct ConListNode_t* i_node = listener->connections->front;
    while(i_node != NULL) {
        result++;
        i_node = i_node->next;
    }
    pthread_mutex_unlock(&listener->connections->lock);
    return result;
}

Connection* getConnection(Listener* listener, int n) {
    Connection* result = NULL;
    pthread_mutex_lock(&listener->connections->lock);
    struct ConListNode_t* i_node = listener->connections->front;
    int i = 0;
    while(i_node != NULL && i < n) {
        i++;
        i_node = i_node->next;
    }
    result = i_node->con;
    pthread_mutex_unlock(&listener->connections->lock);
    return result;
}

timespec_t diff(timespec_t start, timespec_t end) {
    timespec_t temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

timespec_t getCurrentTime() {
    timespec_t time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time);
    return time;
}

void singleConnectionTest() {
    printf("Performing single connection test\n");

    Listener* listener = createListener(PORT,10);
    assert(listener != NULL);

    Connection* con = newClientConnection("127.0.0.1", PORT);
    assert(con != NULL);
    WAITFORCOND(countConnections(listener) == 1,1,"connection failed\n",;)

    Msg client_msg;
    client_msg.buffer = "test_message";
    client_msg.buffer_length = strlen(client_msg.buffer);
    sendConnection(con,client_msg);
    
    Msg recvMsg = dequeueMsgQueue(getReceivedMessages(listener->connections->front->con));
    WAITFORCOND(recvMsg.buffer_length != 0,1,"Client -> Server failed\n",
        recvMsg = dequeueMsgQueue(getReceivedMessages(listener->connections->front->con));
    )
    assert(strcmp(client_msg.buffer,recvMsg.buffer) == 0);

    Msg server_msg;
    server_msg.buffer = "test2_message";
    server_msg.buffer_length = strlen(server_msg.buffer);
    Connection* server_con = getConnection(listener,0);
    sendConnection(server_con,server_msg);
    
    recvMsg = dequeueMsgQueue(getReceivedMessages(con));
    WAITFORCOND(recvMsg.buffer_length != 0,1,"Server -> Client failed\n",
        recvMsg = dequeueMsgQueue(getReceivedMessages(con));
    )
    assert(strcmp(server_msg.buffer,recvMsg.buffer) == 0);

    freeConnection(con);
    WAITFORCOND(countConnections(listener) == 0,1,"Connection free failed\n",;)
    freeListener(listener);
}

void multipleMsgSingleConnectionTest() {
    int port = PORT + 1;
    printf("Performing multiple msg sending with one connection\n");
    Listener* listener = createListener(port,10);
    assert(listener != NULL);

    Connection* con = newClientConnection("127.0.0.1", port);
    assert(con != NULL);
    WAITFORCOND(countConnections(listener) == 1,1,"connection failed\n",;)

    char buffer[17];
    int i = 0;
    Msg recvMsg;
    for(i = 0; i < 1000; i++) {
        sprintf(buffer, "test_message%04d", i);
        Msg client_msg;
        client_msg.buffer = buffer;
        client_msg.buffer_length = strlen(client_msg.buffer);
        sendConnection(con,client_msg);
        
        recvMsg = dequeueMsgQueue(getReceivedMessages(listener->connections->front->con));
        WAITFORCOND(recvMsg.buffer_length != 0,1,"Client -> Server failed\n",
            recvMsg = dequeueMsgQueue(getReceivedMessages(listener->connections->front->con));
        )
        assert(strcmp(client_msg.buffer,recvMsg.buffer) == 0);
    } 
    
    char buffer2[18];
    for(i = 0; i < 1000; i++) {
        sprintf(buffer2, "test2_message%04d", i);
        Msg server_msg;
        server_msg.buffer = buffer2;
        server_msg.buffer_length = strlen(server_msg.buffer);
        Connection* server_con = getConnection(listener,0);
        sendConnection(server_con,server_msg);
        
        recvMsg = dequeueMsgQueue(getReceivedMessages(con));
        WAITFORCOND(recvMsg.buffer_length != 0,1,"Server -> Client failed\n",
            recvMsg = dequeueMsgQueue(getReceivedMessages(con));
        )
        assert(strcmp(server_msg.buffer,recvMsg.buffer) == 0);
    }
    freeConnection(con);
    WAITFORCOND(countConnections(listener) == 0,1,"Connection free failed\n",;)
    freeListener(listener);
    printf("Done\n");
}

int main(int argc, char **argv) {
    if(argc != 2) {
        return 0;
    }
    if(strcmp(argv[1],"single") == 0) {
        singleConnectionTest();
    } else if(strcmp(argv[1],"multipleMsg") == 0) {
        multipleMsgSingleConnectionTest();
    }
    return 0;
}
