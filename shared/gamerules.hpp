#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <thread>
#include <algorithm>

#ifdef CLIENT
#define Gamerules CGamerules
#endif

class Gamerules;
struct SurroundingInfo;

extern "C" {
    #include "../comms/comms.h"
}

#include "shared_enums.hpp"
#include "world.hpp"
#include "player.hpp"
#include "volatile_entities_manager.hpp"
#include "dynamite.hpp"
#include "string_reader.hpp"

#ifdef CLIENT
#include "../client/c_screen.hpp"
#define KEEP_ALIVE_PERIOD 1.0f

struct LobbyClient {
    int id;
    std::string name;
    bool status;
};
#else
#include <list>
#include <map>

#define MAP_FILE "level1.map"
#define ROUND_TIME 90

struct SurroundingInfo {
    struct vectorComp {
        bool operator() (const sf::Vector2<int>& lhs, const sf::Vector2<int>& rhs) const {
            return true;
        }
    };
    sf::Vector2<float> position;
    std::map<sf::Vector2<int>,WorldCell,vectorComp> worldCells;
    std::vector<VolatileEntity*> entities;
    std::vector<Dynamite*> dynamites;
    std::vector<Player*> players;
    sf::Vector2<int> findWorldCell(WorldCell cell);
    bool containsWorldCell(WorldCell cell);
    VolatileEntity* findVolatileEntity(VolatileEntityType type);
};
#endif

class Gamerules {
    GameState state;
    sf::Time endTime;

    //entities
    std::thread mainLoop;
    bool mainLoopOn;
    World* world;
    VolatileEntitiesManager* volatileEntitiesManager;
    std::vector<Player> players;
#ifdef CLIENT
    std::vector<Dynamite> dynamites;
#else
    std::list<Dynamite> dynamites;
#endif

    void handleLobbyState();
    void handleInitState();
    void handleGameState();

    sf::Clock clock;
    sf::Time lastLoopStart;
    sf::Time deltaTime;

    Player* getPlayer(int id);
    int fromFloatToDFloat(float a);
    sf::Vector2<int> getDirectionFromNumber(int i);
    int getNumberFromDirection(sf::Vector2<int> dir);

    std::string getSpecialNullTerminationString(std::string a, int size);
    std::string fromIntegerToBigEndianBytes(int num,int n);
    void sendMessage(Connection* con, std::string message);

#ifdef CLIENT
    Connection* connection;
    int myClientId;
    std::string playerName;
    std::vector<LobbyClient> lobbyClients;
    sf::Time lastReceivedMessage;
    std::vector<int> gameOverWinners;
    std::unique_ptr<CScreen> screen;

    char* m_addr;

    sf::Time lastKeepAlive;
    short lastInputState;

    void toConnectionErrorState();

    std::vector<std::string> getMessages();

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
#else
    Listener* listener;
    sf::Time initStart;
    sf::Time lastLobbyStatus;

    std::map<Connection*,std::vector<std::string>> getMessages();
    void sendMessageForAllPlayers(const std::string& message);

    void sendLobbyStatus();
    void sendGameStart();
    void sendMapUpdate();
    void sendObjects();
    void sendGameOver();
    void sendJoinResponse(int player_id, Connection* con);
    void sendErrorJoinResponse(int code, Connection* con);

    void parseJoinRequest(StringReader& reader,Connection* con);
    void parseKeepAlive(StringReader& reader);
    void parseReady(StringReader& reader);
    void parsePlayerInput(StringReader& reader);
    void parseDisconnect(StringReader& reader);

    void cleanupPlayers();
    unsigned int countReady();
    unsigned int countAlive();

    sf::Rect<float> getSurroundingBox(sf::Vector2<float> _position) {
        return sf::Rect<float>(_position.x - 0.5f,_position.y - 0.5f,
                           _position.x + 0.5f,_position.y + 0.5f);
    }
    std::vector<sf::Vector2<int>> getSurroundingCoords(sf::Rect<float> box);
    void cleanupDynamites();
    int findFreePlayerId();
#endif

public:

#ifdef CLIENT
    Gamerules(std::string addr, int port, std::string _playerName);
#else
    Gamerules(int port);
#endif
    ~Gamerules();

    World* getWorld();
    VolatileEntitiesManager* getVolatileEntitiesManager();
    sf::Time getCurrentTime();
    sf::Time getDeltaTime();
    Dynamite* getDynamiteInPosition(sf::Vector2<int> position);

    void handleMainLoop();
    void parseMessages();
    void cleanup();

    GameState getState() {
        return state;
    }

#ifdef CLIENT
    void ready();
    void disconnectClient();
    void draw(sf::RenderWindow& window);
#else
    Dynamite* createDynamite(sf::Vector2<float> _position, unsigned int _power, Player* _owner);
    SurroundingInfo scanSurrounding(sf::Vector2<float> _position);
#endif
};