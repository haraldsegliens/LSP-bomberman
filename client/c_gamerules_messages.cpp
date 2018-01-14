#include "../shared/gamerules.hpp"

void CGamerules::handleParseMessages() {
    while(true) {
        auto messages = connection.getReceivedMessages();
        for(auto msg : messages) {
            StringReader reader(msg);
            PacketType packetId = (PacketType)reader.getUnsignedChar();
            switch(packetId) {
                case PacketType::JOIN_RESPONSE:
                    parseJoinResponse(reader,pair.first);
                    break;
                case PacketType::LOBBY_STATUS:
                    parseLobbyStatus(reader);
                    break;
                case PacketType::GAME_START:
                    parseGameStart(reader);
                    break;
                case PacketType::MAP_UPDATE:
                    parseMapUpdate(reader);
                    break;
                case PacketType::OBJECTS:
                    parseObjects(reader);
                    break;
                case PacketType::GAME_OVER:
                    parseGameOver(reader);
                    break;
            }
        }
    }
}

void CGamerules::sendJoinRequest() {
    std::string message;
    //TODO: message
    connection->send(message);
}

void CGamerules::sendKeepAlive() {
    std::string message;
    //TODO: message
    connection->send(message);
}

void CGamerules::sendReady() {
    std::string message;
    //TODO: message
    connection->send(message);
}

void CGamerules::sendPlayerInput(short inputState) {
    std::string message;
    //TODO: message
    connection->send(message);
}

void CGamerules::parseJoinResponse(StringReader& reader) {

}

void CGamerules::parseLobbyStatus(StringReader& reader) {

}

void CGamerules::parseGameStart(StringReader& reader) {

}

void CGamerules::parseMapUpdate(StringReader& reader) {

}
void CGamerules::parseObjects(StringReader& reader) {

}
void CGamerules::parseGameOver(StringReader& reader) {

}