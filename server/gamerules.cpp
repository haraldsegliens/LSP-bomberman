#include "../shared/gamerules.hpp"

Gamerules::Gamerules(int port) : listener(port,10), 
                                 mainLoop(&Gamerules::handleMainLoop, this), 
                                 parseMessages(&Gamerules::handleParseMessages, this) {
    cleanup();
}

Gamerules::~Gamerules() {}

void Gamerules::cleanup() {
    state = GameState::LOBBY;
    world.cleanup();
    world.loadMap(loadMapFromFile(MAP_FILE));
    volatileEntityManager.cleanup();
    players.clear();
}

void Gamerules::handleLobbyState() {
    std::unique_lock<std::mutex> lock(mutex, std::defer_lock);
    for(auto& player_it : players) {
        *player_it.updateKeepAlive();
    }

    lock.lock();
    cleanupPlayers();
    lock.unlock();

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
    if(readyCount == players.size() || (getCurrentTime() - initStart).asSeconds() > 10.0f) {
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

void Gamerules::handleParseMessages() {
    
}

std::vector<WorldCell> Gamerules::loadMapFromFile(std::string filename) {
    std::vector<WorldCell> map;
    //load map from space(newline) seperated file
    //first two number mapWidth, mapHeight (Space seperated)
    //newline
    //space seperated list of numbers
    return map;
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

void Gamerules::cleanupPlayers() {
    for(auto it = players.begin(); it != players.end(); ) {
        if(*it.mustDestroy()) {
            it = players.erase(it);
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
