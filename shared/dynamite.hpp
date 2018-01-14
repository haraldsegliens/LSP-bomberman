#ifndef DYNAMITE_H
#define DYNAMITE_H

#include <Vector2.hpp>
#include "shared_enums.hpp"
#include "gamerules.hpp"

#ifdef CLIENT
#define Dynamite CDynamite
#include <RenderWindow.hpp>
#endif

class Dynamite {
    sf::Vector2<float> position;
#ifdef SERVER
    sf::Vector2<int> slideDirection;
    float slideSpeed;
    bool destroy;
    Player* owner;
    sf::Time createdTime;
#endif

public:
#ifdef SERVER
    Dynamite(sf::Vector2<float> _position, Player* _owner);
#else
    Dynamite(sf::Vector2<float> _position);
#endif
    ~Dynamite();

   sf::Vector2<float> getPosition() { return position; } 

#ifdef SERVER
    void kick(sf::Vector2<int> _slideDirection);
    void update(Gamerules* gamerules);
    bool mustDestroy();
    void explode();
#else
    void draw(sf::RenderWindow& window);
#endif
};

#endif