#ifndef GAMERULES_H
#define GAMERULES_H

#include "comms/listener_wrapper.hpp"
#include "shared_enums.hpp"
#include <Clock.hpp>
#include <vector>
#include <pthread.h>

#ifdef CLIENT
#include <Time.hpp>

#define Gamerules CGamerules

struct LobbyClient {
    int id;
    std::string name;
    bool status;
};
#else
#include <Clock.hpp>

#endif

class Gamerules {
    GameState state;
    sf::Time endTime;

    int dynamiteTime;
    float dynamiteSlideSpeed;
    //izveidot visu entītiju deklerācijas
    //std::vector<>
    pthread_mutex_t lock;
    pthread_t mainLoop;
    pthread_t parseMessages;

#ifdef CLIENT
    ConnectionWrapper connection;
    int myClientId;
    std::vector<LobbyClient> lobbyClients;
    sf::Time lastReceivedMessage;
    std::vector<int> gameOverWinners;
    //CScreen screen
#else
    sf::Clock clock;
    ListenerWrapper listener;
    sf::Time initStart;
    sf::Time lastLoopStart;
    float deltaTime;
#endif

public:

#ifdef CLIENT
    Gamerules(std::string addr, int port);
#else
    Gamerules();
#endif
    ~Gamerules();

    void* handleMainLoop();
    void* handleParseMessages();
    void cleanup();

#ifdef CLIENT
    void ready();
    void disconnectClient();
#endif
};

#endif