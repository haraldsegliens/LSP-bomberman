#include "../shared/gamerules.h"

CGamerules::CGamerules(std::string addr, int port) : mainLoop(&CGamerules::handleMainLoop, this), 
                                                     parseMessages(&CGamerules::handleParseMessages, this) {
    cleanup();
    state = GameState::NOT_CONNECTED;
    connection = std::make_unique<ConnectionWrapper>(ConnectionWrapper(addr,port));
    sendJoinRequest();
}

CGamerules::~CGamerules() {}

void CGamerules::cleanup() {
    world.cleanup();
    volatileEntityManager.cleanup();
    players.clear();
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
        toConnectionErrorState();
    } else if(lastInputState != events.inputState) {
        sendPlayerInput(events.inputState);
    }
    handleLobbyState();
}

void CGamerules::handleMainLoop() {
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

void CGamerules::ready() {
    sendReady();
}

void CGamerules::disconnectClient() {

}

void CGamerules::toConnectionErrorState() {
    state = GameState::CONNECTION_ERROR;
    screen.reset(nullptr);
    connection.reset(nullptr);
    cleanup();
}