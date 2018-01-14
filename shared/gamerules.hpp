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

#define MAP_FILE "level1.map"
#define ROUND_TIME 90
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

    void handleLobbyState();
    void handleInitState();
    void handleGameState();

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
    sf::Time deltaTime;

    std::vector<WorldCell> loadMapFromFile(std::string filename);
    void sendMessageForAllPlayers(const std::string& message);
    void sendLobbyStatus();
    void sendGameStart();
    void sendMapUpdate();
    void sendObjects();
    void sendGameOver();
    void cleanupPlayers();
    int countReady();
#endif

public:

#ifdef CLIENT
    Gamerules(std::string addr, int port);
#else
    Gamerules(int port);
#endif
    ~Gamerules();

    void handleMainLoop();
    void handleParseMessages();
    void cleanup();

    World* getWorld() {
        return &world;
    }

#ifdef CLIENT
    void ready();
    void disconnectClient();
#else
    sf::Time getCurrentTime() {
        return clock.getElapsedTime();
    }

    sf::Time getDeltaTime() {
        return deltaTime;
    }
#endif
};

#endif