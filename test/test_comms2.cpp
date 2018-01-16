#include <iostream>

extern "C" {
    #include "../comms/comms.h"
}

#define PORT 1234

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

int main() {
	Listener* listener = createListener(PORT,10);
    if(listener == nullptr) {
    	std::cout << "ERROR 1" << std::endl;
    }
    if(countConnections(listener) != 0) {
    	std::cout << "ERROR 2" << std::endl;
    }
	return 0;
}