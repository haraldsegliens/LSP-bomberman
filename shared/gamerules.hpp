#ifndef GAMERULES_H
#define GAMERULES_H

#include <SFML/Clock.hpp>
#include <vector>
#include <thread>
#include <mutex>

#include "shared_enums.hpp"
#include "world.hpp"
#include "player.hpp"
#include "volatile_entities_manager.hpp"
#include "dynamite.hpp"

extern "C" {
    #include "comms/comms.h"
}

#ifdef CLIENT
#include <SFML/Time.hpp>
#include <memory>

#include "../client/c_screen.hpp"

#define Gamerules CGamerules
#define KEEP_ALIVE_PERIOD 1.0f

struct LobbyClient {
    int id;
    std::string name;
    bool status;
};
#else
#include <SFML/Rect.hpp>
#include <list>
#include <map>

#define MAP_FILE "level1.map"
#define ROUND_TIME 90

struct SurroundingInfo {
    sf::Vector2<float> position;
    std::map<sf::Vector2<int>,WorldCell> worldCells;
    std::vector<VolatileEntity*> entities;
    std::vector<Dynamite*> dynamites;
    std::vector<Player*> players;

    sf::Vector2<int> findWorldCell(WorldCell cell) {
        for(auto& it : worldCells) {
            if(it.second == cell)
                return it.first;
        }
        return sf::Vector2<int>(-1,-1);
    }

    bool containsWorldCell(WorldCell cell) {
        return findWorldCell(cell) != sf::Vector2<int>(-1,-1);
    }

    VolatileEntity* findVolatileEntity(VolatileEntityType type) {
        for(auto& it : entities) {
            if(*it.type == type)
                return &*it;
        }
        return nullptr;
    }
};
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
#ifdef CLIENT
    std::vector<Dynamite> dynamites;
#else
    std::list<Dynamite> dynamites;
#endif

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
    Connection* connection;
    int myClientId;
    std::vector<LobbyClient> lobbyClients;
    sf::Time lastReceivedMessage;
    std::vector<int> gameOverWinners;
    std::unique_ptr<CScreen> screen;

    sf::Time lastKeepAlive;
    short lastInputState;

    void toConnectionErrorState();

    std::vector<std::string>> getMessages();
    void sendMessage(std::string message);

    void sendJoinRequest();
    void sendKeepAlive();
    void sendReady();
    void sendDisconnect();
    void sendPlayerInput(short inputState);

    void parseJoinResponse(StringReader& reader);
    void parseLobbyStatus(StringReader& reader);
    void parseGameStart(StringReader& reader);
    void parseMapUpdate(StringReader& reader);
    void parseObjects(StringReader& reader);
    void parseGameOver(StringReader& reader);

    sf::Texture dynamiteTexture;

    void draw(sf::RenderWindow& window);
#else
    Listener* listener;
    sf::Time initStart;

    std::map<Connection*,std::vector<std::string>> getMessages();
    void sendMessageForAllPlayers(const std::string& message);

    void sendLobbyStatus();
    void sendGameStart();
    void sendMapUpdate();
    void sendObjects();
    void sendGameOver();

    void parseJoinRequest(StringReader& reader,Connection* con);
    void parseKeepAlive(StringReader& reader);
    void parseReady(StringReader& reader);
    void parsePlayerInput(StringReader& reader);
    void parseDisconnect(StringReader& reader);

    void cleanupPlayers();
    int countReady();

    Rect<float> getSurroundingBox(Vector2<float> _position) {
        return Rect<float>(_position.x - 0.5f,_position.y - 0.5f,
                           _position.x + 0.5f,_position.y + 0.5f);
    }
    std::vector<sf::Vector2<int>> getSurroundingCoords(Rect<float> box);
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

    VolatileEntitiesManager* getVolatileEntitiesManager() {
        return &volatileEntityManager;
    }

    sf::Time getCurrentTime() {
        return clock.getElapsedTime();
    }

    sf::Time getDeltaTime() {
        return deltaTime;
    }

    Dynamite* getDynamiteInPosition(sf::Vector2<int> position) {
        for(auto& it : dynamites) {
            if(sf::Vector2<int>(*it.getPosition()) == position) {
                return *it;
            }
        }
        return nullptr;
    }

#ifdef CLIENT
    void ready();
    void disconnectClient();
#else
    Dynamite* createDynamite(sf::Vector2<float> _position, Player* _owner) {
        dynamites.push_back(Dynamite(_position,_owner));
        return &dynamites.back();
    }

    SurroundingInfo scanSurrounding(Gamerules* gamerules, Vector2<float> _position);
#endif
};

#endif