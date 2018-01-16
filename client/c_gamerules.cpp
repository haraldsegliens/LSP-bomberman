#include "../shared/gamerules.hpp"
#include <iostream>

CGamerules::CGamerules(std::string addr, int port, std::string _playerName) : mainLoopOn(true),
                                                                              world(new World()),
                                                                              volatileEntitiesManager(new VolatileEntitiesManager()),
                                                                              playerName(_playerName) {
    cleanup();
    state = GameState::NOT_CONNECTED;
    m_addr = new char[addr.size() + 1];
    strcpy(m_addr,addr.c_str());
    connection = newClientConnection(m_addr,port);
    sendJoinRequest();

    if(!dynamiteTexture.loadFromFile("materials/dynamite.png")) {
        std::cout << "Error loading player texture: " << "materials/dynamite.png" << std::endl;
    }

    mainLoop = std::thread(&CGamerules::handleMainLoop, this);
}

CGamerules::~CGamerules() {
    mainLoopOn = false;
    mainLoop.join();
    freeConnection(connection);
    delete [] m_addr;
}

void CGamerules::cleanup() {
    world->cleanup();
    volatileEntitiesManager->cleanup();
    players.clear();
    dynamites.clear();
    myClientId = -1;
    lastInputState = 0;
    lobbyClients.clear();
    gameOverWinners.clear();
}

void CGamerules::handleLobbyState() {
    if((getCurrentTime() - lastKeepAlive).asSeconds() > KEEP_ALIVE_PERIOD) {
        sendKeepAlive();
        lastKeepAlive = getCurrentTime();
    }

    if((getCurrentTime() - lastReceivedMessage).asSeconds() > TIMEOUT_DURATION) {
        toConnectionErrorState();
    }
}

void CGamerules::handleInitState() {
    WindowEvents events = screen->draw(this);
    if(events.windowClosed) {
        toConnectionErrorState();
    }
}

void CGamerules::handleGameState() {
    WindowEvents events = screen->draw(this);
    if(events.windowClosed) {
        sendDisconnect();
    } else if(lastInputState != events.inputState) {
        sendPlayerInput(events.inputState);
    }
    handleLobbyState();
}

void CGamerules::ready() {
    sendReady();
}

void CGamerules::disconnectClient() {
    sendDisconnect();
    toConnectionErrorState();
    state = GameState::END;
}

void CGamerules::toConnectionErrorState() {
    state = GameState::CONNECTION_ERROR;
    screen.reset(nullptr);
    freeConnection(connection);
    connection = nullptr;
    cleanup();
}

void CGamerules::draw(sf::RenderWindow& window) {
    world->draw(window);
    volatileEntitiesManager->draw(window);
    for(Dynamite& dynamite : dynamites) {
        dynamite.draw(window);
    }
    for(Player& player : players) {
        player.draw(window);
    }
}