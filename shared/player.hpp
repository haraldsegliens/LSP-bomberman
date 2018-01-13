#ifndef PLAYER_H
#define PLAYER_H

#include <string.h>
#include <Time.hpp>
#include <Vector2.hpp>
#include "shared_enums.hpp"

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
    std::list<VolatileEntity*> currentDynamites;
    sf::Time startedTemporaryPowerup;
    int playerInput;
    int lastProcessedPlayerInput;
#endif
public:
    Player();
    ~Player();

#ifdef CLIENT
    void draw(sf::RenderWindow& window);
#else
    void updateKeepAlive();
    void update();
    void destroy();
#endif
};

#endif
