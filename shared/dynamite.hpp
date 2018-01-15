#ifndef DYNAMITE_H
#define DYNAMITE_H

#include <SFML/Vector2.hpp>
#include "shared_enums.hpp"
#include "gamerules.hpp"

#ifdef CLIENT
#define Dynamite CDynamite
#include <SFML/RenderWindow.hpp>
#endif

class Dynamite {
    sf::Vector2<float> position;
#ifdef SERVER
    sf::Vector2<float> slideDirection;
    float slideSpeed;
    bool destroy;
    Player* owner;
    sf::Time explosionTime;
    int power;
#else
    sf::Texture* dynamiteTexture;
#endif

public:
#ifdef SERVER
    Dynamite(sf::Vector2<float> _position,int _power,Player* _owner, sf::Time _explosionTime);
#else
    Dynamite(sf::Vector2<float> _position, sf::Texture* _dynamiteTexture);
#endif
    ~Dynamite();

   sf::Vector2<float> getPosition() { return position; } 

#ifdef SERVER
    void kick(sf::Vector2<int> _slideDirection);
    void update(Gamerules* gamerules);
    bool mustDestroy() { return destroy; }
    void explode();
#else
    void draw(sf::RenderWindow& window);
#endif
};

#endif