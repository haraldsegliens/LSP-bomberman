#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "../comms/comms.h"

#define PORT 9123

int countConnections(Listener* listener) {
    int result = 0;
    struct ConListNode_t* i_node = listener->connecions->front;
    while(i_node != NULL) {
        result++;
        i_node = i_node->next;
    }
    return result;
}

int main() {
    printf("Performing connection test");

    Listener* listener = createListener(int port);
    assert(listener == NULL);
    printf("Created listener\n");

    Connection* con = newConnection("127.0.0.1", PORT);
    assert(con == NULL);
    sleep(1);
    assert(countConnections(listener) == 1);
    printf("Created connection\n");

    Msg msg;
    msg.buffer = "test_message";
    msg.buffer_length = strlen(msg.buffer);
    sendConnection(con,msg);
    sleep(1);
    Msg recvMsg = dequeueMsgQueue(getReceivedMessages(listener->connections->front->con));
    assert(strcmp(msg.buffer,recvMsg.buffer) == 0);
    printf("Connection -> Listener\n");

    freeConnection(con);
    freeListener(listener);
    return 0;
}