#include "../shared/gamerules.hpp"

void Gamerules::handleParseMessages() {
    while(true) {
        auto messages = listener.getReceivedMessages();
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

void Gamerules::sendMessageForAllPlayers(const std::string& message) {
    for(auto& player_it : players) {
        *player_it.getConnection()->send(message);
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

void Gamerules::parseJoinRequest(StringReader& reader,ConnectionWrapper* con) {
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