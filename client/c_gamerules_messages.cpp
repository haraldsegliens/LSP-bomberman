#include "../shared/gamerules.hpp"
#include <iostream>

void CGamerules::parseMessages() {
    if(connection == nullptr) {
        toConnectionErrorState();
        return;
    }

    auto messages = getMessages();
    for(auto msg : messages) {
        StringReader reader(msg);
        PacketType packetId = (PacketType)reader.getBinaryNumber(1);
        std::cout << "Packet: " << packetId << std::endl;
        switch(packetId) {
            case PacketType::JOIN_RESPONSE:
                parseJoinResponse(reader);
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
            default:
                break;
        }
    }
}

void CGamerules::sendJoinRequest() {
    if(playerName.length() < 23) {
        playerName += '\0';
    }

    std::string message;
    message += (char)PacketType::JOIN_REQUEST;
    message += playerName.substr(0,23);
    sendMessage(connection,message);
}

void CGamerules::sendKeepAlive() {
    std::string message;
    message += (char)PacketType::KEEP_ALIVE;
    message += (char)myClientId;
    sendMessage(connection,message);
}

void CGamerules::sendReady() {
    std::string message;
    message += (char)PacketType::READY;
    message += (char)myClientId;
    sendMessage(connection,message);
}

void CGamerules::sendDisconnect() {
    std::string message;
    message += (char)PacketType::DISCONNECT;
    message += (char)myClientId;
    sendMessage(connection,message);
}

void CGamerules::sendPlayerInput(short inputState) {
    std::string message;
    message += (char)PacketType::PLAYER_INPUT;
    message += (char)myClientId;
    message += fromIntegerToBigEndianBytes((int)inputState,2);
    sendMessage(connection,message);
}

void CGamerules::parseJoinResponse(StringReader& reader) {
    if(state == GameState::NOT_CONNECTED) {
        auto code = reader.getBinaryNumber(1);
        switch(code) {
            case 0:
                std::cout << "LOBBY state" << std::endl;
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
        std::cout << "INIT state" << std::endl;
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
        world->loadMap(map,sf::Vector2<int>(width,height));
        volatileEntitiesManager->load(this);
        screen = std::unique_ptr<CScreen>(new CScreen());

        sendReady();
        lastReceivedMessage = getCurrentTime();
    }
}

void CGamerules::parseMapUpdate(StringReader& reader) {
    if(state == GameState::INIT) {
        std::cout << "GAME state" << std::endl;
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
            world->changeCell(sf::Vector2<int>(_x,_y),(WorldCell)_value);
        }
        lastReceivedMessage = getCurrentTime();
    }
}

void CGamerules::parseObjects(StringReader& reader) {
    if(state == GameState::INIT) {
        std::cout << "GAME state" << std::endl;
        state = GameState::GAME;
    }
    if(state == GameState::GAME) {
        dynamites.clear();

        //int timer = reader.getBinaryNumber(2);
        endTime = getCurrentTime() + sf::seconds(reader.getBinaryNumber(2));

        int dynamiteCount = reader.getBinaryNumber(1);
        for(int i = 0; i < dynamiteCount; i++) {
            float _x = reader.getDFloat(2);
            float _y = reader.getDFloat(2);
            CDynamite dynamite(sf::Vector2<float>(_x,_y), &dynamiteTexture);
            dynamites.push_back(dynamite);
        }
        volatileEntitiesManager->clear();
        int fireCount = reader.getBinaryNumber(1);
        for(int i = 0; i < fireCount; i++) {
            int _x = reader.getBinaryNumber(1);
            int _y = reader.getBinaryNumber(1);
            volatileEntitiesManager->get(sf::Vector2<int>(_x,_y))->type = VolatileEntityType::FIRE;
        }
        int powerupCount = reader.getBinaryNumber(1);
        for(int i = 0; i < powerupCount; i++) {
            int _x = reader.getBinaryNumber(1);
            int _y = reader.getBinaryNumber(1);
            Powerup _powerup = (Powerup)reader.getBinaryNumber(1);
            VolatileEntity* entity = volatileEntitiesManager->get(sf::Vector2<int>(_x,_y));
            entity->type = VolatileEntityType::FIRE;
            entity->powerupType = _powerup;
        }

        int playerCount = reader.getBinaryNumber(1);
        std::vector<int> playerIds;
        for(int i = 0; i < playerCount; i++) {
            int _id = reader.getBinaryNumber(1);
            playerIds.push_back(_id);
            Player* player = getPlayer(_id);

            bool _dead = reader.getBinaryNumber(1) == 1;
            player->setDead(true);

            if(!_dead) {
                float _x = reader.getDFloat(2);
                float _y = reader.getDFloat(2);
                player->setPosition(sf::Vector2f(_x,_y));

                int _dir = reader.getBinaryNumber(1);
                player->setDirection(getDirectionFromNumber(_dir));
                player->setPowerup((Powerup)reader.getBinaryNumber(1));
                player->setPower(reader.getBinaryNumber(1));
                player->setSpeed(reader.getBinaryNumber(1));
                player->setMaxDynamiteCount(reader.getBinaryNumber(1));
            }
        }

        //setting dead those players that werent found in this packet
        auto it = players.begin();
        while(it != players.end()) {
            if(std::find(playerIds.begin(),playerIds.end(),it->getId()) == playerIds.end()) {
                it->setDead(true);
            }
        }

        lastReceivedMessage = getCurrentTime();
    }
}

void CGamerules::parseGameOver(StringReader& reader) {
    state = GameState::END;
    std::cout << "END" << std::endl;
    //read winners
}

std::vector<std::string> Gamerules::getMessages() {
    std::vector<std::string> connection_messages;
    MsgQueue* messages = getReceivedMessages(connection);
    auto j_node = messages->front;
    while(j_node != nullptr) {
        std::string message(j_node->message.buffer,j_node->message.buffer_length);
        connection_messages.push_back(message);
        j_node = j_node->next;
    }
    return connection_messages;
}