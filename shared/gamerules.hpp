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

#define TIMEOUT_DURATION 10.0f

#ifdef CLIENT
#include <Time.hpp>
#include <memory>

#include "comms/connection_wrapper.hpp"
#include "../client/c_screen.hpp"

#define Gamerules CGamerules
#define KEEP_ALIVE_PERIOD 1.0f

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

    sf::Clock clock;
    sf::Time lastLoopStart;
    sf::Time deltaTime;

#ifdef CLIENT
    std::unique_ptr<ConnectionWrapper> connection;
    int myClientId;
    std::vector<LobbyClient> lobbyClients;
    sf::Time lastReceivedMessage;
    std::vector<int> gameOverWinners;
    std::unique_ptr<CScreen> screen;

    sf::Time lastKeepAlive;
    short lastInputState;

    void toConnectionErrorState();

    void sendJoinRequest();
    void sendKeepAlive();
    void sendPlayerInput(short inputState);

    void parseJoinResponse(StringReader& reader);
    void parseLobbyStatus(StringReader& reader);
    void parseGameStart(StringReader& reader);
    void parseMapUpdate(StringReader& reader);
    void parseObjects(StringReader& reader);
    void parseGameOver(StringReader& reader);

#else
    ListenerWrapper listener;
    sf::Time initStart;

    std::vector<WorldCell> loadMapFromFile(std::string filename);
    void sendMessageForAllPlayers(const std::string& message);

    void sendLobbyStatus();
    void sendGameStart();
    void sendMapUpdate();
    void sendObjects();
    void sendGameOver();

    void parseJoinRequest(StringReader& reader,ConnectionWrapper* con);
    void parseKeepAlive(StringReader& reader);
    void parseReady(StringReader& reader);
    void parsePlayerInput(StringReader& reader);
    void parseDisconnect(StringReader& reader);

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

    sf::Time getCurrentTime() {
        return clock.getElapsedTime();
    }

    sf::Time getDeltaTime() {
        return deltaTime;
    }

#ifdef CLIENT
    void ready();
    void disconnectClient();
#endif
};

#endif