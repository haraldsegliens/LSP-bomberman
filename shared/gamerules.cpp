#include "gamerules.hpp"

void Gamerules::handleMainLoop() {
    while(mainLoopOn) {
        deltaTime = getCurrentTime() - lastLoopStart;
        lastLoopStart += deltaTime;
        switch(state) {
            case GameState::LOBBY:
                handleLobbyState();
                break;
            case GameState::INIT:
                handleInitState();
                break;
            case GameState::GAME:
                handleGameState();
                break;
            default:
                break;
        }
        parseMessages();
    }
}

World* Gamerules::getWorld() {
    return &*world;
}

VolatileEntitiesManager* Gamerules::getVolatileEntitiesManager() {
    return &*volatileEntitiesManager;
}

sf::Time Gamerules::getCurrentTime() {
    return clock.getElapsedTime();
}

sf::Time Gamerules::getDeltaTime() {
    return deltaTime;
}

Dynamite* Gamerules::getDynamiteInPosition(sf::Vector2<int> position) {
    for(Dynamite& dynamite : dynamites) {
        if(sf::Vector2<int>(dynamite.getPosition()) == position) {
            return &dynamite;
        }
    }
    return nullptr;
}

Player* Gamerules::getPlayer(int id) {
    for(Player& player : players) {
        if(player.getId() == id) {
            return &player;
        }
    }
    return nullptr;
}

int Gamerules::fromFloatToDFloat(float a) {
    return static_cast<int>(a * 10.0f);
}

sf::Vector2<int> Gamerules::getDirectionFromNumber(int i) {
    if(i == 0) {
        return sf::Vector2<int>(0,-1);
    } else if(i == 1) {
        return sf::Vector2<int>(1,0);
    } else if(i == 2) {
        return sf::Vector2<int>(0,1);
    } else {
        return sf::Vector2<int>(-1,0);
    }
}

int Gamerules::getNumberFromDirection(sf::Vector2<int> dir) {
    if(dir.x == 0) {
        if(dir.y == 1) {
            return 2;
        } else {
            return 0;
        }
    } else {
        if(dir.x == 1) {
            return 1;
        } else {
            return 3;
        }
    }
}

std::string Gamerules::from2ByteIntegerToString(int i) {
    std::string a;
    a += (char)(i & 0xff);
    a += (char)((i >> 8) & 0xff);
    return a;
}