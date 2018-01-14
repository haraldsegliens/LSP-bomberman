#ifndef PLAYER_H
#define PLAYER_H

#include <string.h>
#include <Time.hpp>
#include <Vector2.hpp>
#include "shared_enums.hpp"
#include "gamerules.hpp"
#include <vector>

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
    sf::Vector2<float> position;
    sf::Vector2<int> direction;
    Powerup powerup;
    int power;
    int speed;
    int maxDynamiteCount;

#ifdef CLIENT
    sf::Texture playerTexture;
#else
    typedef enum {
        KEY_PRESSED
        NONE

    } InputBitState;

    Connection* playerConnection;
    sf::Time lastKeepAlive;
    bool ready;
    bool dead;
    bool destroy;
    std::list<Dynamite*> currentDynamites;
    sf::Time endTemporaryPowerup;
    short playerInput;
    short lastProcessedPlayerInput;

    void handleSurroundings(Gamerules* gamerules, SurroundingInfo& info);
    void handlePlayerInput(Gamerules* gamerules, SurroundingInfo& info);
    void movePlayer(Gamerules* gamerules);
    float getUnitSpeed() {
        return PLAYER_BASE_SPEED + PLAYER_INCREASE_SPEED * speed;
    }

    bool isBitSet(short value,PlayerInputBits bit) {
        return (bool)(value >> bit) & 1U;
    }

    bool isOneTimePressed(PlayerInputBits bit) {
        return isBitSet(playerInput,bit) && !isBitSet(lastProcessedPlayerInput,bit);
    }

    bool isPressed(PlayerInputBits bit) {
        return isBitSet(playerInput,bit)
    }

#endif
public:
#ifdef SERVER
    Player(int _id,std::string _name,sf::Vector2<float> _position,Connection* _con);
#else
    Player(int _id,std::string _name,sf::Vector2<float> _position);
#endif
    ~Player();

    bool isReady() { return ready; }
    void setReady(bool ready) { this->ready = ready; }

    Vector2f getPosition() {return position;}

#ifdef CLIENT
    void draw(sf::RenderWindow& window);
#else
    void updateKeepAlive(Gamerules* gamerules);
    void update(Gamerules* gamerules);

    bool mustDestroy() { return destroy; }
    bool isDead() { return dead; }
    Connection* getConnection() { return playerConnection; }
    bool isTemporaryPowerup() { return powerup == Powerup::REMOTE_DETONATOR || powerup == Powerup::DYNAMITE_KICK; }

    void removeDynamite(Dynamite* dynamite);
#endif
};

#endif
