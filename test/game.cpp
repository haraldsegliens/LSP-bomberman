#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <bitset>
#include <unistd.h>

using namespace std;

char myPlayerId;

class Player {
    // int id;
    // std::string name;
    // Vector2<float> position;
    // Vector2<int> direction;
    // Powerup powerup;
    // int power;
    // int speed;
    // int maxDynamiteCount;

    sf::Texture playerTexture;
    // ConnectionWrapper* playerConnection;
    sf::Time lastKeepAlive;
    bool ready;
    bool dead;
    bool destroy;
    // std::list<VolatileEntity*> currentDynamites;
    sf::Time startedTemporaryPowerup;
    short playerInput;
    short lastProcessedPlayerInput;


public:
    short id;
    std::string name;
    short x;
    short y;
    char direction;
    short power;
    short speed;
    short maxDynamiteCount;
    char activePowerup;

    // Player();
    // ~Player();

    bool isReady() { return ready; }
    void setReady(bool ready) { this->ready = ready; }

    void draw(sf::RenderWindow& window);
    // void updateKeepAlive(Gamerules* gamerules);
    // void update(Gamerules* gamerules);

    bool mustDestroy() { return destroy; }
    bool isDead() { return dead; }
    // ConnectionWrapper* getConnection() { return playerConnection; }

    void setId(short id) { this->id = id; }
    void setName(char* name) { this->name = name; }
    void setPosition(short x, short y) { this->x = x; this->y = y; }
    void setDirection(char direction) { this->direction = direction; }
    void setActivePowerUp(char powerUp) { this->activePowerup = powerUp; }
    void setPower(short power) { this->power = power; }
    void setSpeed(short speed) { this->speed = speed; }
    void setDynamiteCount(short count) { this->maxDynamiteCount = count; }
    void setDead() { this->dead = true; }
};

class World 
{
    public:
        int sizeX;
        int sizeY;
        vector<vector<short>> worldTiles;
        char dynamiteTime;
        char dynamiteSpeed;

        void setSizeX(char sizeX) { this->sizeX = sizeX; }
        void setSizeY(char sizeY) { this->sizeY = sizeY;  }
        void setWorldTiles(vector<vector<short>> &worldTiles) { this->worldTiles = worldTiles; }
        void setDynamiteTime(char dynamiteTime) { this->dynamiteTime = dynamiteTime; }
        void setDynamiteSpeed(char dynamiteSpeed) { this->dynamiteSpeed = dynamiteSpeed; }
        void updateTile(short tileX, short tileY, short newValue ) { this->worldTiles[tileX][tileY] = newValue; }
};

World world;
vector<Player> players;
vector<short> results;

int connectToServer(const char* IP, int PORT)
{
    // copied from 
    // https://www.geeksforgeeks.org/socket-programming-cc/
    ///////////////////////////////////////////////////////
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        // socket connection problem
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, IP, &serv_addr.sin_addr)<=0) 
    {
        // invalid address
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        // connection failed
        return -1;
    }

    return sock;
}

void sendJoinRequest(int socket, string playerName)
{
    // cropping player input name to allowed size
    if(playerName.length() < 23)
    {
        playerName += '\0';
    }
    string request; 
    request += (char)(1);
    request += playerName.substr(0,23);
    send(socket, request.data(), request.length(), 0);
}

void sendKeepAlive(int socket, char playerId)
{
    string request;
    request += (char)3;
    request += playerId;
    send(socket, request.data(), 2, 0);
}

void sendReady(int socket, char playerId)
{
    string request;
    request += (char)4;
    request += playerId;
    send(socket, request.data(), 2, 0);
}

// checking from sfml what keys are pressed, creates short integer to send in packet
short bindActionToShort(sf::Event::KeyEvent keyPressed)
{
    bitset<16> action;
    action.reset();
    sf::Keyboard keyboard;
    switch (keyPressed.code)
    {
        case keyboard.Up :
            action.set(0);
        case keyboard.Down :
            action.set(1);
        case keyboard.Left :
            action.set(2);
        case keyboard.Right :
            action.set(3);
        case keyboard.Space :
            action.set(4);
        case keyboard.R :
            action.set(5);
        case keyboard.E :
            action.set(6);
        // case keyboard. :
        //     action.set(7);
        // case keyboard. :
        //     action.set(8);
        case keyboard.T :
            action.set(9);
        case keyboard.D :
            action.set(10);
        case keyboard.L :
            action.set(11);
        case keyboard.Num1 :
            action.set(12);
        case keyboard.Num2 :
            action.set(13);
        case keyboard.Num3 :
            action.set(14);
        case keyboard.Num4 :
            action.set(15);
    }
    short short_action = (short)(action.to_ulong());
    
    return short_action;

}

void sendPlayerInput(int socket, char playerId, short action)
{
    string request;
    request += (char)5;
    request += playerId;
    request += (action & 0xff);
    request += ((action >> 8) & 0xff);
    send(socket, request.data(), 4, 0);
}

void disconnect(int socket, char playerId)
{
    string request;
    request += (char)6;
    request += playerId;
    send(socket, request.data(), 2, 0);
}

void setMyPlayerId(char playerId)
{
    myPlayerId = playerId;
}

void readJoinResponse(int socket)
{
    int valread;
    char responseCode[1] = {0};
    valread = read(socket, responseCode, 1);
    switch ((int)responseCode[0])
    {
        case 0 :
        {
            char playerId[1] = {0};
            valread = read(socket, playerId, 1);
            // set player ID
            setMyPlayerId(playerId[0]);
            break;
        }
        case 1 :
        {
            // server is now running a game
            break;
        }
        case 2 :
        {
            // server is full
            break;
        }
    }
}

void updateLobbyClient(char playerId, char* playerName, bool readyStatus)
{
    for (int i = 0; i < players.size(); ++i)
    {
        if(players[i].id == (short)playerId)
        {
            players[i].setName(playerName);
            players[i].setReady(readyStatus);

            return;
        }
        // inserting new player
        Player new_player;
        new_player.setId((short)playerId);
        new_player.setName(playerName);
        new_player.setReady(readyStatus);
        players.push_back(new_player);
    }
}

void readLobbyStatusResponse(int socket)
{
    int valread;
    char playerCount[1] = {0};
    valread = read(socket, playerCount, 1);
    for (int i = 0; i < (int)playerCount[0]; ++i)
    {
        bool readyStatusBool = false;
        char playerId[1] = {0};
        valread = read(socket, playerId, 1);
        char playerName[23] = {0};
        valread = read(socket, playerName, 23);
        char readyStatus[1] = {0};
        valread = read(socket, readyStatus, 1);
        if((int)readyStatus[0] == 1)
        {
            readyStatusBool = true;
        }
        // updating lobby clients
        updateLobbyClient(playerId[0], playerName, readyStatusBool);
    }
}

void updateLobbyClientGameStart(char playerId, char* playerName, short x, short y, char direction)
{
    for (int i = 0; i < players.size(); ++i)
    {
        if(players[i].id == (short)playerId)
        {
            players[i].setName(playerName);
            players[i].setReady(true);

            return;
        }
        // inserting new player
        Player new_player;
        new_player.setId((short)playerId);
        new_player.setName(playerName);
        new_player.setReady(true);
        new_player.setPosition(x, y);
        new_player.setDirection(direction);
        players.push_back(new_player);
    }
}

void createWorldGameStart(char sizeX, char sizeY, vector<vector<short>> & worldTiles, char dynamiteTime, char dynamiteSpeed)
{
    world.setSizeX(sizeX);
    world.setSizeY(sizeY);
    world.setWorldTiles(worldTiles);
    world.setDynamiteTime(dynamiteTime);
    world.setDynamiteSpeed(dynamiteSpeed);
}

void readGameStartResponse(int socket)
{
    int valread;
    char playerCount[1] = {0};
    valread = read(socket, playerCount, 1);
    for (int i = 0; i < (int)playerCount[0]; ++i)
    {
        char playerId[1] = {0};
        valread = read(socket, playerId, 1);
        char playerName[23] = {0};
        valread = read(socket, playerName, 23);
        short x;
        valread = read(socket, &x, 2);
        short y;
        valread = read(socket, &y, 2);
        char direction[1] = {0};
        valread = read(socket, direction, 1);
        // updating clients before game
        updateLobbyClientGameStart(playerId[0], playerName, x, y, direction[0]);
    }
    vector<vector<short>> worldTiles;
    char worldX[1] = {0};
    valread = read(socket, worldX, 1);
    char worldY[1] = {0};
    valread = read(socket, worldY, 1);
    for (int i = 0; i < (short)worldX[0]; ++i)
    {
        for (int j = 0; j < (short)worldY[0]; ++j)
        {
            char tile[1] = {0};
            valread = read(socket, tile, 1);
            worldTiles[(short)i][(short)j] = (short)tile[0];
        }
    }
    char dynamiteTime[1] = {0};
    valread = read(socket, dynamiteTime, 1);
    char dynamiteSpeed[1] = {0};
    valread = read(socket, dynamiteSpeed, 1);
    createWorldGameStart(worldX[0], worldY[0], worldTiles, dynamiteTime[0], dynamiteSpeed[0]);
}

void readMapUpdateResponse(int socket)
{
    int valread;
    short changes;
    valread = read(socket, &changes, 2);
    for (int i = 0; i < changes; ++i)
    {
        char tileX[1] = {0};
        valread = read(socket, tileX, 1);
        char tileY[1] = {0};
        valread = read(socket, tileY, 1);
        char newValue[1] = {0};
        valread = read(socket, newValue, 1);
        world.updateTile((short)tileX[0], (short)tileY[0], (short)newValue[0]);
    }
}

void updateGameTimer(short time)
{

}

void addDynamites(short x, short y)
{

}

void addFlames(short x, short y)
{

}

void addPowerUps(short x, short y, short type)
{

}

void updateAlivePlayer(char playerId, short x, short y, char direction, char activePowerup, char power, char speed, char dynamiteCount)
{
    for (int i = 0; i < players.size(); ++i)
    {
        if(players[i].id == (short)playerId)
        {
            players[i].setPosition(x, y);
            players[i].setDirection(direction);
            players[i].setActivePowerUp(activePowerup);
            players[i].setPower((short)power);
            players[i].setSpeed((short)speed);
            players[i].setDynamiteCount((short)dynamiteCount);

            return;
        }
    }
}

void updateDeadPlayer(char playerId)
{
    for (int i = 0; i < players.size(); ++i)
    {
        if(players[i].id == (short)playerId)
        {
            players[i].setDead();

            return;
        }
    }
}

void readObjectsResponse(int socket)
{
    int valread;
    short gameTimer;
    valread = read(socket, &gameTimer, 2);
    updateGameTimer(gameTimer);
    // reading and updating dynamites
    char dynamiteCount[1] = {0};
    valread = read(socket, dynamiteCount, 1);
    for (int i = 0; i < (short)dynamiteCount[0]; ++i)
    {
        short x;
        valread = read(socket, &x, 2);
        short y;
        valread = read(socket, &y, 2);
        addDynamites(x, y);
    }
    // reading and updating flames
    char flameCount[1] = {0};
    valread = read(socket, flameCount, 1);
    for (int i = 0; i < (short)flameCount[0]; ++i)
    {
        char x[1] = {0};
        valread = read(socket, x, 1);
        char y[1] = {0};
        valread = read(socket, y, 1);
        addFlames((short)x[0], (short)y[0]);
    }
    // reading and updating power ups
    char powerUpCount[1] = {0};
    valread = read(socket, powerUpCount, 1);
    for (int i = 0; i < (short)powerUpCount[0]; ++i)
    {
        char x[1] = {0};
        valread = read(socket, x, 1);
        char y[1] = {0};
        valread = read(socket, y, 1);
        char type[1] = {0};
        valread = read(socket, type, 1);
        addPowerUps((short)x[0], (short)y[0], (short)type[0]);
    }
    // reading and updating player statuses
    char playerCount[1] = {0};
    valread = read(socket, playerCount, 1);
    for (int i = 0; i < (short)playerCount[0]; ++i)
    {
        char playerId[1] = {0};
        valread = read(socket, playerId, 1);
        char playerDead[1] = {0};
        if((short)playerDead[0] == 0)
        {
            valread = read(socket, playerDead, 1);
            short x;
            valread = read(socket, &x, 2);
            short y;
            valread = read(socket, &y, 2);
            char direction[1] = {0};
            valread = read(socket, direction, 1);
            char activePowerup[1] = {0};
            valread = read(socket, activePowerup, 1);
            char power[1] = {0};
            valread = read(socket, power, 1);
            char speed[1] = {0};
            valread = read(socket, speed, 1);
            char dynamiteCount[1] = {0};
            valread = read(socket, dynamiteCount, 1);
            // updating player when its alive
            updateAlivePlayer(playerId[0], x, y, direction[0], activePowerup[0], power[0], speed[0], dynamiteCount[0]);
        }
        else
        {
            updateDeadPlayer(playerId[0]);
        }
    }
}

void readGameOverResponse(int socket)
{
    int valread;
    char playerCount[1] = {0};
    valread = read(socket, playerCount, 1);
    for (int i = 0; i < (int)playerCount[0]; ++i)
    {
        char playerId[1] = {0};
        valread = read(socket, playerId, 1);
        results.push_back((short)playerId[0]);
    }
}

int main()
{
    const char* IP = "127.0.0.1";
    const int PORT = 8080;

    int socket = connectToServer(IP, PORT);

    sf::RenderWindow window(sf::VideoMode(800, 800), "BOMBERMAN");

    char playerId = 'X';

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                // window closed
                case sf::Event::Closed:
                {
                    window.close();

                    break;
                }
                case sf::Event::KeyPressed:
                {
                    // user is giving some input
                    short action = bindActionToShort(event.key);
                    sendPlayerInput(socket, playerId, action);
                }
                default:
                    break;
            }
        }
        window.clear(sf::Color::Black);

        window.display();
    }

    return 0;
}