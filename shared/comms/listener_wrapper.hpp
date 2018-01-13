#ifndef LISTENER_WRAPPER_H
#define LISTENER_WRAPPER_H

#include <string.h>
#include <vector>
#include "connection_wrapper.hpp"

extern "C" {
    #include "../../comms/comms.h"
}

class ListenerWrapper {
	Listener* m_listener;
public:
    ListenerWrapper(int port, int connections_count);
    ~ListenerWrapper();
    //redzēs kādas funkcijas vajadzēs
};

#endif

