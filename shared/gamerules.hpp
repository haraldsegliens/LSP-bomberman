#ifndef GAMERULES_H
#define GAMERULES_H

#include <Clock.hpp>
#include <vector>
#include <thread>
#include <mutex>

#include "shared_enums.hpp"
#include "world.hpp"
#include "player.hpp"
#include "volatile_entities_manager.hpp"

#ifdef CLIENT
#include <Time.hpp>

#include "comms/connection_wrapper.hpp"
#include "../client/c_screen.hpp"

#define Gamerules CGamerules

struct LobbyClient {
    int id;
    std::string name;
    bool status;
};
#else
#include <Clock.hpp>

#include "comms/listener_wrapper.hpp"
#endif

class Gamerules {
    GameState state;
    sf::Time endTime;

    int dynamiteTime;
    float dynamiteSlideSpeed;
    //entities
    World world;
    VolatileEntityManager volatileEntityManager;
    std::vector<Player> players;

    //threads and mutex
    std::mutex mutex;
    std::thread mainLoop;
    std::thread parseMessages;

#ifdef CLIENT
    ConnectionWrapper connection;
    int myClientId;
    std::vector<LobbyClient> lobbyClients;
    sf::Time lastReceivedMessage;
    std::vector<int> gameOverWinners;
    CScreen screen;
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