#include "../shared/gamerules.hpp"
#include <iostream>

void Gamerules::parseMessages() {
    std::map<Connection*,std::vector<std::string>> messages = getMessages();
    for(auto pair : messages) {
        for(std::string msg : pair.second) {
            StringReader reader(msg);
            PacketType packetId = (PacketType)reader.getBinaryNumber(1);
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
                default:
                    break;
            }
        }
    }
}

void Gamerules::sendLobbyStatus() {
    std::string message;
    message += (char)PacketType::LOBBY_STATUS;
    message += (char)players.size();
    for(Player& player : players) {
        auto name = player.getName().substr(0,23);
        if(name.length() < 23) {
            name += '\0';
        }

        message += (char)player.getId();
        message += name;
        message += (char)(player.isReady() ? 0 : 1);
    }
    sendMessageForAllPlayers(message);
}

void Gamerules::sendGameStart() {
    std::string message;
    message += (char)PacketType::GAME_START;
    message += (char)players.size();
    for(Player& player : players) {
        message += (char)player.getId();
        message += getSpecialNullTerminationString(player.getName(),23);
        int _x = fromFloatToDFloat(player.getPosition().x);
        message += fromIntegerToBigEndianBytes(_x,2);
        int _y = fromFloatToDFloat(player.getPosition().y);
        message += fromIntegerToBigEndianBytes(_y,2);
        message += (char)getNumberFromDirection(player.getDirection());
    }
    message += (char)world->getWidth();
    message += (char)world->getHeight();
    int i_max = world->getWidth() * world->getHeight();
    for(int i = 0; i < i_max; i++) {
        message += (char)world->getCell(i);
    }
    sendMessageForAllPlayers(message);
}

void Gamerules::sendMapUpdate() {
    std::vector<WorldChange> changes = world->popChanges();
    if(changes.size() == 0) {
        return;
    }
    std::string message;
    message += (char)PacketType::MAP_UPDATE;
    message += fromIntegerToBigEndianBytes(changes.size(),2);
    for(WorldChange change : changes) {
        message += (char)change.position.x;
        message += (char)change.position.y;
        message += (char)change.value;
    }

    sendMessageForAllPlayers(message);
}

void Gamerules::sendGameOver() {
    std::string message;
    message += (char)PacketType::GAME_OVER;
    message += (char)players.size();
    for(Player& player : players) {
        message += (char)player.getId();//for now invalid version of this packet
    }
    sendMessageForAllPlayers(message);
}

void Gamerules::sendObjects() {
    std::string message;
    message += (char)PacketType::OBJECTS;
    int timer = static_cast<int>((endTime - getCurrentTime()).asSeconds());
    message += fromIntegerToBigEndianBytes(timer,2);
    message += (char)dynamites.size();
    for(Dynamite& dynamite : dynamites) {
        int _x = fromFloatToDFloat(dynamite.getPosition().x);
        int _y = fromFloatToDFloat(dynamite.getPosition().y);
        message += fromIntegerToBigEndianBytes(_x,2);
        message += fromIntegerToBigEndianBytes(_y,2);
    }
    std::vector<sf::Vector2i> fireEntityPositions = volatileEntitiesManager->getAllPositionsWithType(VolatileEntityType::FIRE);
    message += (char)fireEntityPositions.size();
    for(sf::Vector2i fireEntityPositions : fireEntityPositions) {
        message += (char)fireEntityPositions.x;
        message += (char)fireEntityPositions.y;
    }

    std::vector<sf::Vector2i> powerupEntityPositions = volatileEntitiesManager->getAllPositionsWithType(VolatileEntityType::POWERUP);
    message += (char)powerupEntityPositions.size();
    for(sf::Vector2i powerupEntityPosition : powerupEntityPositions) {
        VolatileEntity* entity = volatileEntitiesManager->get(powerupEntityPosition);
        message += (char)powerupEntityPosition.x;
        message += (char)powerupEntityPosition.y;
        message += (char)entity->powerupType;
    }

    message += (char)players.size();
    for(Player& player : players){
        message += (char)player.getId();
        message += (char)player.isDead();
        if(!player.isDead()) {
            int _x = fromFloatToDFloat(player.getPosition().x);
            int _y = fromFloatToDFloat(player.getPosition().y);
            message += fromIntegerToBigEndianBytes(_x,2);
            message += fromIntegerToBigEndianBytes(_y,2);
            int _dir = getNumberFromDirection(player.getDirection());
            message += (char)_dir;
            message += (char)player.getPowerup();
            message += (char)player.getPower();
            message += (char)player.getSpeed();
            message += (char)player.getMaxDynamiteCount();
        }
    }
    sendMessageForAllPlayers(message);
}

void Gamerules::sendJoinResponse(int player_id, Connection* con) {
    std::string message;
    message += (char)PacketType::JOIN_RESPONSE;
    message += (char)0;
    message += (char)player_id;
    sendMessage(con,message);
}

void Gamerules::sendErrorJoinResponse(int code, Connection* con) {
    std::string message;
    message += (char)PacketType::JOIN_RESPONSE;
    message += (char)code;
    sendMessage(con,message);
}

void Gamerules::parseJoinRequest(StringReader& reader,Connection* con) {
    if(state == GameState::LOBBY) {
        if(players.size() < 4) {
            std::string name = reader.get();
            int id = findFreePlayerId();
            Player player(id,name,sf::Vector2f(world->getSpawnPoint(id)),con,this);
            players.push_back(player);
            sendJoinResponse(id,con);
        } else {
            sendErrorJoinResponse(2,con);
        }
    } else {
        sendErrorJoinResponse(1,con);
    }
}

void Gamerules::parseKeepAlive(StringReader& reader) {
    int player_id = reader.getBinaryNumber(1);
    for(Player& player : players){
        if(player.getId() == player_id) {
            player.setKeepAliveTime(getCurrentTime());
            return;
        }
    }
}

void Gamerules::parseReady(StringReader& reader) {
    if(state == GameState::LOBBY || state == GameState::INIT) {
        int player_id = reader.getBinaryNumber(1);
        Player* player = getPlayer(player_id);
        if(player != nullptr) {
            player->setReady(true);
        }
    }
}

void Gamerules::parsePlayerInput(StringReader& reader) {
    if(state == GameState::GAME) {
        int player_id = reader.getBinaryNumber(1);
        unsigned short inputState = reader.getBinaryNumber(2);
        Player* player = getPlayer(player_id);
        if(player != nullptr) {
            player->setInputState(inputState);
            std::cout << inputState << " ";
        }
    }
}

void Gamerules::parseDisconnect(StringReader& reader) {
    int player_id = reader.getBinaryNumber(1);
    Player* player = getPlayer(player_id);
    if(player != nullptr) {
        player->setDestroy(true);
    }
}

void Gamerules::sendMessageForAllPlayers(const std::string& message) {
    for(Player& player : players) {
        sendMessage(player.getConnection(), message);
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
            connection_messages.push_back(std::string(j_node->message.buffer,
                                                      j_node->message.buffer_length));
            j_node = j_node->next;
        }
        all_messages.insert(std::pair<Connection*,
                            std::vector<std::string>>(i_node->con,connection_messages));
        i_node = i_node->next;
    }
    pthread_mutex_unlock(&listener->connections->lock);
    return all_messages;
}