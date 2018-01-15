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
    bool dead;

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
    Player(int _id,std::string _name,sf::Vector2<float> _position,sf::Vector2<int> _direction, int order);
#endif
    ~Player();

    bool isReady() { return ready; }
    void setReady(bool ready) { this->ready = ready; }

    int getId() { return id; } 

    std::string const& getName() { return name; }

    sf::Vector2f getPosition() {return position;}
    void setPosition(sf::Vector2f a) {
        position = a;
    }

    sf::Vector2i getDirection() {return direction;}
    void setDirection(sf::Vector2i a) {
        direction = a;
    }

    Powerup getPowerup() {return powerup;}
    void setPowerup(Powerup a) {
        powerup = a;
    }

    int getPower() {return power;}
    void setPower(int a) {
        power = a;
    }

    int getSpeed() {return speed;}
    void setSpeed(int a) {
        speed = a;
    }

    int getMaxDynamiteCount() {return maxDynamiteCount;}
    void setMaxDynamiteCount(int a) {
        maxDynamiteCount = a;
    }

    bool isDead() { return dead; }
    bool setDead(bool a) { dead = a; }

#ifdef CLIENT
    void draw(sf::RenderWindow& window);
#else
    void updateKeepAlive(Gamerules* gamerules);
    void update(Gamerules* gamerules);

    void setKeepAliveTime(sf::Time time) {
        lastKeepAlive = time;
    }

    void setDestroy(bool _destroy) { destroy = _destroy; }
    bool mustDestroy() { return destroy; }
    Connection* getConnection() { return playerConnection; }
    bool isTemporaryPowerup() { return powerup == Powerup::REMOTE_DETONATOR || powerup == Powerup::DYNAMITE_KICK; }

    void removeDynamite(Dynamite* dynamite);

    bool isReady() {return ready;}
    void setReady(bool a) {
        ready = a;
    }

    void setInputState(short a) {
        playerInput = a;
    }
#endif
};

#endif
