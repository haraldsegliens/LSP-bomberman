#include "../shared/gamerules.hpp"

void Gamerules::handleParseMessages() {
    while(true) {
        std::map<Connection*,std::vector<std::string>> messages = getMessages();
        for(auto pair : messages) {
            for(auto msg : messages.second) {
                StringReader reader(msg);
                PacketType packetId = (PacketType)reader.getUnsignedChar();
                switch(packetId) {
                    case PacketType::JOIN_REQUEST:
                        parseJoinRequest(reader,pair.first);
                        break;
                    case PacketType::KEEP_ALIVE:
                        parseKeepAlive(reader);
                        break;
                    case PacketType::READY:
                        parseReady(reader);
                        break;
                    case PacketType::PLAYER_INPUT:
                        parsePlayerInput(reader);
                        break;
                    case PacketType::DISCONNECT:
                        parseDisconnect(reader);
                        break;
                }
            }
        }
    }
}

void Gamerules::sendLobbyStatus() {
    std::string message;
    //TODO: sagatavot ziņojumu
    sendMessageForAllPlayers(message);
}

void Gamerules::sendGameStart() {
    std::string message;
    //TODO: sagatavot ziņojumu
    sendMessageForAllPlayers(message);
}

void Gamerules::sendMapUpdate() {
    std::string message;
    //TODO: sagatavot ziņojumu
    sendMessageForAllPlayers(message);
}

void Gamerules::sendGameOver() {
    std::string message;
    //TODO: sagatavot ziņojumu
    sendMessageForAllPlayers(message);
}

void Gamerules::sendObjects() {
    std::string message;
    //TODO: sagatavot ziņojumu
    sendMessageForAllPlayers(message);
}

void Gamerules::parseJoinRequest(StringReader& reader,Connection* con) {
    //TODO: noparsēt ziņojumu
}

void Gamerules::parseKeepAlive(StringReader& reader) {
    //TODO: noparsēt ziņojumu
}

void Gamerules::parseReady(StringReader& reader) {
    //TODO: noparsēt ziņojumu
}

void Gamerules::parsePlayerInput(StringReader& reader) {
    //TODO: noparsēt ziņojumu
}

void Gamerules::parseDisconnect(StringReader& reader) {
    //TODO: noparsēt ziņojumu
}

void Gamerules::sendMessageForAllPlayers(const std::string& message) {
    for(auto& player_it : players) {
        *player_it.getConnection()->send(message);
    }
}

std::map<Connection*,std::vector<std::string>> Gamerules::getMessages() {
    std::map<Connection*,std::vector<std::string>> all_messages;
    pthread_mutex_lock(&listener->connections->lock);
    auto i_node = listener->connections->front;
    while(i_node != nullptr) {
        std::vector<std::string> connection_messages;
        MsgQueue* messages = getReceivedMessages(i_node->con);
        auto j_node = messages->front;
        while(j_node != nullptr) {
            connection_messages.push_back(std::string(j_node->buffer,j_node->buffer_length));
            j_node = jnode->next;
        }
        all_messages.insert(std::pair<Connection*,
                            std::vector<std::string>>(i_node->con,connection_messages));
        i_node = i_node->next;
    }
    pthread_mutex_unlock(&listener->connections->lock);
    return all_messages;
}