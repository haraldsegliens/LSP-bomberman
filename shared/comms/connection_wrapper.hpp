#ifndef CONNECTION_WRAPPER_H
#define CONNECTION_WRAPPER_H

#include <string.h>
#include <vector>

extern "C" {
    #include "../../comms/comms.h"
}

class ConnectionWrapper {
    Connection* m_connection;
public:
    ConnectionWrapper(Connection* con);
    ConnectionWrapper(std::string addr, int port);
    ~ConnectionWrapper();
    void send(std::string message);
    std:vector<std::string> getReceivedMessages();
};

#endif
