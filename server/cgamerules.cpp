#include "../shared/gamerules.hpp"
#include "../shared/string_reader.hpp"
#include <math.h>


Gamerules::Gamerules(int port) : mainLoopOn(true),
                                 world(new World()),
                                 volatileEntitiesManager(new VolatileEntitiesManager()) {
    listener = createListener(port,10);
    cleanup();
    mainLoop = std::thread(&Gamerules::handleMainLoop, this);
}

Gamerules::~Gamerules() {
    mainLoopOn = false;
    mainLoop.join();
    freeListener(listener);
}

void Gamerules::cleanup() {
    state = GameState::LOBBY;
    world->cleanup();
    volatileEntitiesManager->cleanup();
    players.clear();
    dynamites.clear();
}

void Gamerules::handleLobbyState() {
    for(Player& player : players) {
        player.updateKeepAlive(this);
    }

    cleanupPlayers();
    cleanupDynamites();

    sendLobbyStatus();
    unsigned int readyCount = countReady();
    if(players.size() >= 2 && readyCount == players.size()) {
        sendGameStart();
        state = GameState::INIT;
        initStart = getCurrentTime();
        for(Player& player : players) {
            player.setReady(false);
        }
    }
}

void Gamerules::handleInitState() {
    unsigned int readyCount = countReady();
    if(readyCount == players.size() || (getCurrentTime() - initStart).asSeconds() > TIMEOUT_DURATION) {
        if(readyCount < players.size()) {
            //destroy players that aren't ready
            for(auto it = players.begin(); it != players.end(); ) {
                if(!it->isReady()) {
                    it = players.erase(it);
                } else {
                    it++;
                }
            }
        }

        if(players.size() < 2) {
            sendGameOver();
            cleanup();
        } else {
           state = GameState::GAME;
           endTime = getCurrentTime() + sf::seconds(ROUND_TIME);
        }
    }
}

void Gamerules::handleGameState() {
    world->update(this);
    volatileEntitiesManager->update(this);
    for(Dynamite& dynamite : dynamites) {
        dynamite.update(this);
    }
    
    for(Player& player : players) {
        player.update(this);
    }

    sendMapUpdate();
    sendObjects();

    if(!world->isClosing() && endTime < getCurrentTime()) {
        world->startClosing();
    }

    int alive = countAlive();
    if(alive <= 1) {
        sendGameOver();
        cleanup();
    }
}

void Gamerules::cleanupPlayers() {
    for(auto it = players.begin(); it != players.end(); ) {
        if(it->mustDestroy()) {
            it = players.erase(it);
        } else {
            it++;
        }
    }
}

void Gamerules::cleanupDynamites() {
    for(auto it = dynamites.begin(); it != dynamites.end(); ) {
        if(it->mustDestroy()) {
            it->getOwner()->removeDynamite(&*it);
            it = dynamites.erase(it);
        } else {
            it++;
        }
    }
}

unsigned int Gamerules::countReady() {
    unsigned int readyCount = 0;
    for(Player& player : players) {
        if(player.isReady()) {
            readyCount++;
        }
    }
    return readyCount;
}

unsigned int Gamerules::countAlive() {
    unsigned int aliveCount = 0;
    for(Player& player : players) {
        if(!player.isDead()) {
            aliveCount++;
        }
    }
    return aliveCount;
}

std::vector<sf::Vector2<int>> Gamerules::getSurroundingCoords(sf::Rect<float> box) {
    std::vector<sf::Vector2<int>> coords;
    int x_max = (int)ceil(box.left+box.width);
    int y_max = (int)ceil(box.top+box.height);
    for(int x = (int)box.left; x < x_max; x++) {
        for(int y = (int)box.top; y < y_max; y++) {
            coords.push_back(sf::Vector2<int>(x,y));
        }
    }
    return coords;
}

SurroundingInfo Gamerules::scanSurrounding(Gamerules* gamerules, sf::Vector2<float> _position) {
    sf::Rect<float> box = getSurroundingBox(_position);
    std::vector<sf::Vector2<int>> coords = getSurroundingCoords(box);
    SurroundingInfo info;
    info.position = _position;
    for(sf::Vector2<int> coord : coords) {
        info.worldCells.insert(std::pair<sf::Vector2<int>,WorldCell>(
            coord,gamerules->getWorld()->getCell(coord)
        ));
        VolatileEntity* entity = gamerules->getVolatileEntitiesManager()->get(coord);
        if(entity != nullptr) {
            info.entities.push_back(entity);
        }
    }

    for(Dynamite& dynamite : dynamites) {
        if(box.intersects(getSurroundingBox(dynamite.getPosition()))){
           info.dynamites.push_back(&dynamite);
        }
    }

    for(Player& player : players) {
        if(box.intersects(getSurroundingBox(player.getPosition()))){
           info.players.push_back(&player);
        }
    }
    return info;
}

int Gamerules::findFreePlayerId() {
    for(int i = 0; i < 4; i++) {
        bool found = false;
        for(Player& player : players) {
            if(player.getId() == i) {
                found = true;
                break;
            }
        }
        if(!found) {
            return i;
        }
    }
    return 5;
}

Dynamite* Gamerules::createDynamite(sf::Vector2<float> _position, unsigned int _power, Player* _owner) {
    dynamites.push_back(Dynamite(_position,_power,_owner, getCurrentTime() + sf::seconds(DYNAMITE_DURATION)));
    return &dynamites.back();
}

sf::Vector2<int> SurroundingInfo::findWorldCell(WorldCell cell) {
    for(auto& it : worldCells) {
        if(it.second == cell)
            return it.first;
    }
    return sf::Vector2<int>(-1,-1);
}

bool SurroundingInfo::containsWorldCell(WorldCell cell) {
    return findWorldCell(cell) != sf::Vector2<int>(-1,-1);
}

VolatileEntity* SurroundingInfo::findVolatileEntity(VolatileEntityType type) {
    for(VolatileEntity* entity : entities) {
        if(entity->type == type)
            return entity;
    }
    return nullptr;
}
