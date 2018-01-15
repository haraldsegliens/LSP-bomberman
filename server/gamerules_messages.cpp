#include "../shared/gamerules.hpp"

void Gamerules::parseMessages() {
    std::map<Connection*,std::vector<std::string>> messages = getMessages();
    for(auto pair : messages) {
        for(auto msg : messages.second) {
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
            }
        }
    }
}

void Gamerules::sendLobbyStatus() {
    std::string message;
    message += (char)PacketType::LOBBY_STATUS;
    message += (char)players.size();
    for(auto& it : players) {
        auto name = *it.getName().substr(0,23);
        if(name.length() < 23) {
            name += '\0';
        }

        message += (char)*it.getId();
        message += name;
        message += (char)(*it.isReady() ? 0 : 1);
    }
    sendMessageForAllPlayers(message);
}

void Gamerules::sendGameStart() {
    std::string message;
    message += (char)PacketType::GAME_START;
    message += (char)players.size();
    for(auto& it : players) {
        auto name = *it.getName().substr(0,23);
        if(name.length() < 23) {
            name += '\0';
        }
        message += (char)*it.getId();
        message += name;
        int _x = fromFloatToDFloat(*it.getPosition().x);
        message += from2ByteIntegerToString(_x);
        int _y = fromFloatToDFloat(*it.getPosition().y);
        message += from2ByteIntegerToString(_y);
        message += (char)getNumberFromDirection(*it->getDirection());
    }
    message += (char)world.getWidth();
    message += (char)world.getHeight();
    int i_max = world.getWidth() * world.getHeight();
    for(int i = 0; i < i_max; i++) {
        message += (char)world.getCell(i);
    }
    sendMessageForAllPlayers(message);
}

void Gamerules::sendMapUpdate() {
    std::vector<WorldChange> changes = world.popChanges();

    std::string message;
    message += (char)PacketType::MAP_UPDATE;
    message += from2ByteIntegerToString(changes.size());
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
    message += from2ByteIntegerToString(timer);
    message += (char)dynamites.size();
    for(Dynamite& dynamite : dynamites) {
        int _x = fromFloatToDFloat(dynamite.getPosition().x);
        int _y = fromFloatToDFloat(dynamite.getPosition().y);
        message += from2ByteIntegerToString(_x);
        message += from2ByteIntegerToString(_y);
    }
    std::vector<sf::Vector2i> fireEntityPositions = volatileEntityManager.getAllPositionsWithType(VolatileEntityType::FIRE);
    message += (char)fireEntityPositions.size();
    for(sf::Vector2i fireEntityPositions : fireEntityPositions) {
        message += (char)fireEntityPositions.x;
        message += (char)fireEntityPositions.y;
    }

    std::vector<sf::Vector2i> powerupEntityPositions = volatileEntityManager.getAllPositionsWithType(VolatileEntityType::POWERUP);
    message += (char)powerupEntityPositions.size();
    for(sf::Vector2i powerupEntityPosition : powerupEntityPositions) {
        VolatileEntity* entity = volatileEntityManager.get(powerupEntityPosition);
        message += (char)powerupEntityPosition.x;
        message += (char)powerupEntityPosition.y;
        message += (char)entity.powerupType;
    }

    message += (char)players.size();
    for(Player& player : players){
        message += (char)player.getId();
        message += (char)player.isDead();
        if(!player.isDead()) {
            int _x = fromFloatToDFloat(player.getPosition().x);
            int _y = fromFloatToDFloat(player.getPosition().y);
            message += from2ByteIntegerToString(_x);
            message += from2ByteIntegerToString(_y);
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
            Player player(id,name,Vector2f(world.getSpawnPoint(id)),con);
            players.push_back(player);
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
        for(Player& player : players){
            if(player.getId() == player_id) {
                player.setReady(true);
                return;
            }
        }
    }
}

void Gamerules::parsePlayerInput(StringReader& reader) {
    if(state == GameState::GAME) {
        int player_id = reader.getBinaryNumber(1);
        short inputState = reader.getBinaryNumber(2);
        for(Player& player : players){
            if(player.getId() == player_id) {
                player.setInputState(inputState);
                return;
            }
        }
    }
}

void Gamerules::parseDisconnect(StringReader& reader) {
    int player_id = reader.getBinaryNumber(1);
    for(Player& player : players){
        if(player.getId() == player_id) {
            player.setDestroy(true);
            return;
        }
    }
}

void Gamerules::sendMessage(Connection* con, std::string message) {
    Msg msg;
    msg.buffer = message.c_str();
    msg.buffer_length = message.size();
    sendConnection(con,msg);
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