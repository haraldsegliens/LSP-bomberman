#include "../shared/gamerules.hpp"
#include "../shared/string_reader.hpp"
#include <math.h>

Gamerules::Gamerules(int port) : mainLoop(&Gamerules::handleMainLoop, this), 
                                 parseMessages(&Gamerules::handleParseMessages, this) {
    listener = createListener(port,10)
    cleanup();
}

Gamerules::~Gamerules() {
    freeListener(listener);
}

void Gamerules::cleanup() {
    state = GameState::LOBBY;
    world.cleanup();
    world.loadMapFromFile(MAP_FILE);
    volatileEntityManager.cleanup();
    players.clear();
    dynamites.clear();
}

void Gamerules::handleLobbyState() {
    std::unique_lock<std::mutex> lock(mutex, std::defer_lock);
    for(auto& player_it : players) {
        *player_it.updateKeepAlive();
    }

    lock.lock();
    cleanupPlayers();
    lock.unlock();

    cleanupDynamites();

    sendLobbyStatus();
    int readyCount = countReady();
    if(players.size() >= 2 && readyCount == players.size()) {
        sendGameStart();
        state = GameState::INIT;
        initStart = getCurrentTime();
        for(auto& player_it : players) {
            *player_it.setReady(false);
        }
    }
}

void Gamerules::handleInitState() {
    std::unique_lock<std::mutex> lock(mutex, std::defer_lock);
    int readyCount = countReady();
    if(readyCount == players.size() || (getCurrentTime() - initStart).asSeconds() > TIMEOUT_DURATION) {
        if(readyCount < players.size()) {
            lock.lock();
            //destroy players that aren't ready
            for(auto it = players.begin(); it != players.end(); ) {
                if(!*it.isReady()) {
                    it = players.erase(it);
                } else {
                    it++;
                }
            }
            lock.unlock();
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
    std::unique_lock<std::mutex> lock(mutex, std::defer_lock);
    world.update(this);
    volatileEntityManager.update(this);
    for(auto& dynamite_it : dynamites) {
        *dynamite_it.update(this);
    }
    
    lock.lock();
    for(auto& player_it : players) {
        *player_it.update(this);
    }
    lock.unlock();

    sendMapUpdate();
    sendObjects();

    if(!world.isClosing() && endTime < getCurrentTime()) {
        world.startClosing();
    }

    int countAlive = countAlive();
    if(countAlive <= 1) {
        sendGameOver();
        cleanup();
    }
}

void Gamerules::handleMainLoop() {
    while(true) {
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
        }
    }
}

void Gamerules::cleanupPlayers() {
    for(auto it = players.begin(); it != players.end(); ) {
        if(*it.mustDestroy()) {
            it = players.erase(it);
        } else {
            it++;
        }
    }
}

void Gamerules::cleanupDynamites() {
    for(auto it = dynamites.begin(); it != dynamites.end(); ) {
        if(*it.mustDestroy()) {
            *it.owner->removeDynamite(*it);
            it = dynamites.erase(it);
        } else {
            it++;
        }
    }
}

int Gamerules::countReady() {
    int readyCount = 0;
    for(auto& player_it : players) {
        if(*player_it.isReady()) {
            readyCount++;
        }
    }
    return readyCount;
}

int Gamerules::countAlive() {
    int aliveCount = 0;
    for(auto& player_it : players) {
        if(!*player_it.isDead()) {
            aliveCount++;
        }
    }
    return aliveCount;
}

std::vector<sf::Vector2<int>> Gamerules::getSurroundingCoords(Rect<float> box) {
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

SurroundingInfo Gamerules::scanSurrounding(Gamerules* gamerules, Vector2<float> _position) {
    Rect<float> box = getSurroundingBox(_position)
    std::vector<sf::Vector2<int>> coords = getSurroundingCoords(box);
    info.position = _position;
    SurroundingInfo info;
    for(auto& it : coords) {
        info.worldCells.insert(std::pair<sf::Vector2<int>,WorldCell>(
            *it,gamerules->getWorld()->getCell(*it)
        ));
        VolatileEntity* entity = gamerules->getVolatileEntitiesManager()->get(*it);
        if(entity != nullptr) {
            info.entities.push_back(entity);
        }
    }

    for(auto& it : dynamites) {
        if(box.intersects(getSurroundingBox(*it.getPosition()))){
           info.dynamites.push_back(*it);
        }
    }

    for(auto& it : players) {
        if(box.intersects(getSurroundingBox(*it.getPosition()))){
           info.players.push_back(*it);
        }
    }
    return info;
}
