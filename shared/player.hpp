#ifndef PLAYER_H
#define PLAYER_H

#include <string.h>
#include <Time.hpp>
#include <Vector2.hpp>
#include "shared_enums.hpp"
#include "gamerules.hpp"

#ifdef CLIENT
#define Player CPlayer

#include <RenderWindow.hpp>
#include <Texture.hpp>
#else
#include <list>
#include "volatile_entities_manager.hpp"
#endif

class Player {
    int id;
    std::string name;
    Vector2<float> position;
    Vector2<int> direction;
    Powerup powerup;
    int power;
    int speed;
    int maxDynamiteCount;

#ifdef CLIENT
    sf::Texture playerTexture;
#else
    ConnectionWrapper* playerConnection;
    sf::Time lastKeepAlive;
    bool ready;
    bool dead;
    bool destroy;
    std::list<VolatileEntity*> currentDynamites;
    sf::Time startedTemporaryPowerup;
    short playerInput;
    short lastProcessedPlayerInput;
#endif
public:
    Player();
    ~Player();

    bool isReady() { return ready; }
    void setReady(bool ready) { this->ready = ready; }

#ifdef CLIENT
    void draw(sf::RenderWindow& window);
#else
    void updateKeepAlive(Gamerules* gamerules);
    void update(Gamerules* gamerules);

    bool mustDestroy() { return destroy; }
    bool isDead() { return dead; }
    ConnectionWrapper* getConnection() { return playerConnection; }
#endif
};

#endif
