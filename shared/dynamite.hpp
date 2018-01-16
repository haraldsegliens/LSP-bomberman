#pragma once

#include <SFML/Graphics.hpp>
#include "shared_enums.hpp"

#ifdef CLIENT
#define Dynamite CDynamite
#endif

class Dynamite;

#include "gamerules.hpp"
#include "player.hpp"

class Dynamite {
    sf::Vector2<float> position;
#ifdef SERVER
    sf::Vector2<float> slideDirection;
    unsigned int power;
    float slideSpeed;
    bool destroy;
    Player* owner;
    sf::Time explosionTime;
#else
    sf::Texture* dynamiteTexture;
#endif

public:
#ifdef SERVER
    Dynamite(sf::Vector2<float> _position,unsigned int _power,Player* _owner, sf::Time _explosionTime);
#else
    Dynamite(sf::Vector2<float> _position, sf::Texture* _dynamiteTexture);
#endif
    ~Dynamite();

   sf::Vector2<float> getPosition() { return position; } 

#ifdef SERVER
    void kick(sf::Vector2<int> _slideDirection);
    void update(Gamerules* gamerules);
    bool mustDestroy() { return destroy; }
    void explode(Gamerules* gamerules);
    Player* getOwner() { return owner; }
#else
    void draw(sf::RenderWindow& window);
#endif
};