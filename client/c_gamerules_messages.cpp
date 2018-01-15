#include "../shared/gamerules.hpp"
#include <iostream>

void CGamerules::parseMessages() {
    while(true) {
        auto messages = getMessages();
        for(auto msg : messages) {
            StringReader reader(msg);
            PacketType packetId = (PacketType)reader.getBinaryNumber(1);
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
    if(playerName.length() < 23) {
        playerName += '\0';
    }

    std::string message;
    message += (char)PacketType.JOIN_REQUEST;
    message += playerName.substr(0,23);
    sendMessage(message);
}

void CGamerules::sendKeepAlive() {
    std::string message;
    message += (char)PacketType.KEEP_ALIVE;
    message += (char)myClientId;
    sendMessage(message);
}

void CGamerules::sendReady() {
    std::string message;
    message += (char)PacketType.READY;
    message += (char)myClientId;
    sendMessage(message);
}

void CGamerules::sendDisconnect() {
    std::string message;
    message += (char)PacketType.DISCONNECT;
    message += (char)myClientId;
    sendMessage(message);
}

void CGamerules::sendPlayerInput(short inputState) {
    std::string message;
    message += (char)PacketType.PLAYER_INPUT;
    message += (char)myClientId;
    message += from2ByteIntegerToString((int)inputState)
    sendMessage(message);
}

void CGamerules::parseJoinResponse(StringReader& reader) {
    if(state == GameState::NOT_CONNECTED) {
        auto code = reader.getBinaryNumber(1);
        switch(code) {
            case 0:
                state = GameState::LOBBY;
                myClientId = (int)reader.getBinaryNumber(1);
                lastReceivedMessage = getCurrentTime();
                break;
            case 1:
                std::cout << "parseJoinResponse: Server is busy!" << std::endl;
                toConnectionErrorState();
                break;
            case 2:
                std::cout << "parseJoinResponse: Server is full!" << std::endl;
                toConnectionErrorState();
                break;
            default:
                std::cout << "parseJoinResponse: Other error!" << std::endl;
                toConnectionErrorState();
                break;
        }
    }
}

void CGamerules::parseLobbyStatus(StringReader& reader) {
    if(state == GameState::LOBBY) {
        lobbyClients.clear();

        int playerCount = (int)reader.getBinaryNumber(1);
        for(int i = 0; i < playerCount; i++) {
            LobbyClient client;
            client.id = (int)reader.getBinaryNumber(1);
            client.name = reader.getNullFixedString(23);
            client.status = reader.getBinaryNumber(1) == 1;
            lobbyClients.push_back(client);
        }
        lastReceivedMessage = getCurrentTime();
    }
}

void CGamerules::parseGameStart(StringReader& reader) {
    if(state == GameState::LOBBY) {
        std::cout << "parseGameStart started" << std::endl;
        state = GameState::INIT;
        int playerCount = reader.getBinaryNumber(1);
        std::cout << "reading " << playerCount << " players" << std::endl;
        for(int i = 0; i < playerCount; i++) {
            int _id = reader.getBinaryNumber(1);
            std::string _name = reader.getNullFixedString(23);
            float _x = reader.getDFloat(2);
            float _y = reader.getDFloat(2);
            int _direction = reader.getBinaryNumber(1);
            CPlayer player(_id,_name,sf::Vector2<float>(_x,_y),getDirectionFromNumber(_direction),i);
            players.push_back(player);
            std::cout << "Player: " << _name << std::endl; 
        }

        int width = reader.getBinaryNumber(1);
        int height = reader.getBinaryNumber(1);
        std::cout << "reading world(" << width << "," << height << ")" << std::endl;
        std::vector<WorldCell> map;
        int i_max = width * height;
        for(int i = 0; i < i_max; i++) {
            WorldCell cell = (WorldCell)reader.getBinaryNumber(1);
            map.push_back(cell);
        }
        world.loadMap(map,sf::Vector2<int>(width,height));
        volatileEntityManager.load(this);
        screen == std::make_unique(CScreen());

        sendReady();
        lastReceivedMessage = getCurrentTime();
    }
}

void CGamerules::parseMapUpdate(StringReader& reader) {
    if(state == GameState::INIT) {
        state = GameState::GAME;
    }
    if(state == GameState::GAME) {
        std::cout << "parseMapUpdate started" << std::endl;
        int changeCount = reader.getBinaryNumber(2);
        for(int i = 0; i < changeCount; i++) {
            int _x = reader.getBinaryNumber(1);
            int _y = reader.getBinaryNumber(1);
            int _value = reader.getBinaryNumber(1);
            std::cout << "change_" << i << ": (" << _x << "," << _y << ") -> " << _value << std::endl;
            world.changeCell(sf::Vector2<int>(_x,_y),_value);
        }
        lastReceivedMessage = getCurrentTime();
    }
}

void CGamerules::parseObjects(StringReader& reader) {
    if(state == GameState::INIT) {
        state = GameState::GAME;
    }
    if(state == GameState::GAME) {
        dynamites.clear();

        int timer = reader.getBinaryNumber(2);
        endTime = getCurrentTime() + sf::seconds(reader.getBinaryNumber(2));

        int dynamiteCount = reader.getBinaryNumber(1);
        for(int i = 0; i < dynamiteCount; i++) {
            float _x = reader.getDFloat(2);
            float _y = reader.getDFloat(2);
            CDynamite dynamite(sf::Vector2<float>(_x,_y), &dynamiteTexture);
            dynamites.push_back(dynamite);
        }
        volatileEntityManager.clear();
        int fireCount = reader.getBinaryNumber(1);
        for(int i = 0; i < fireCount; i++) {
            int _x = reader.getBinaryNumber(1);
            int _y = reader.getBinaryNumber(1);
            volatileEntityManager.get(sf::Vector2<int>(_x,_y))->type = VolatileEntityType::FIRE;
        }
        int powerupCount = reader.getBinaryNumber(1);
        for(int i = 0; i < powerupCount; i++) {
            int _x = reader.getBinaryNumber(1);
            int _y = reader.getBinaryNumber(1);
            Powerup _powerup = (Powerup)reader.getBinaryNumber(1);
            VolatileEntity* entity = volatileEntityManager.get(sf::Vector2<int>(_x,_y));
            entity->type = VolatileEntityType::FIRE;
            entity->powerupType = _powerup;
        }

        int playerCount = reader.getBinaryNumber(1);
        std::vector<int> playerIds;
        for(int i = 0; i < powerupCount; i++) {
            int _id = reader.getBinaryNumber(1);
            playerIds.push_back(playerIds);
            Player* player = getPlayer(_id);

            bool _dead = reader.getBinaryNumber(1) == 1;
            player.setDead(true);

            if(!_dead) {
                float _x = reader.getDFloat(2);
                float _y = reader.getDFloat(2);
                player->setPosition(sf::Vector2f(_x,_y));

                int _dir = reader.getBinaryNumber(1);
                player->setDirection(getDirectionFromNumber(_direction));
                player->setPowerup((Powerup)reader.getBinaryNumber(1));
                player->setPower(reader.getBinaryNumber(1));
                player->setSpeed(reader.getBinaryNumber(1));
                player->setMaxDynamiteCount(reader.getBinaryNumber(1));
            }
        }

        //setting dead those players that werent found in this packet
        auto it = players.begin();
        while(it != players.end()) {
            if(std::find(playerIds.begin(),playerIds.end(),*it.getId()) == playerIds.end()) {
                *it->setDead(true);
            }
        }

        lastReceivedMessage = getCurrentTime();
    }
}

void CGamerules::parseGameOver(StringReader& reader) {
    state = GameState::END;
    std::cout << "Game over" << std::endl;
    //read winners
}

std::vector<std::string> Gamerules::getMessages() {
    std::vector<std::string> connection_messages;
    MsgQueue* messages = getReceivedMessages(connection);
    auto j_node = messages->front;
    while(j_node != nullptr) {
        connection_messages.push_back(std::string(j_node->buffer,j_node->buffer_length));
        j_node = jnode->next;
    }
    return connection_messages;
}

void Gamerules::sendMessage(std::string message) {
    Msg msg;
    msg.buffer = message.c_str();
    msg.buffer_length = message.size();
    sendConnection(connection,msg);
}