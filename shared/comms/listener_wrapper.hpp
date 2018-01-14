#ifndef LISTENER_WRAPPER_H
#define LISTENER_WRAPPER_H

#include <string.h>
#include <list>
#include <map>
#include "connection_wrapper.hpp"

extern "C" {
    #include "../../comms/comms.h"
}

class ListenerWrapper {
	Listener* m_listener;
	std::list<ConnectionWrapper> connections;
public:
    ListenerWrapper(int port, int connections_count);
    ~ListenerWrapper();
    std::list<ConnectionWrapper>& getConnections();//read connections from listener, then return
    void removeConnection(ConnectionWrapper* connection);
    std::map<ConnectionWrapper*,std:vector<std::string>> getReceivedMessages();
};

#endif

