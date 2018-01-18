#include "../shared/dynamite.hpp"

Dynamite::Dynamite(sf::Vector2<float> _position, unsigned int _power,
                   Player* _owner, sf::Time _explosionTime) : position(_position),
                                                              power(_power),
                                                              slideSpeed(0.0f),
                                                              destroy(false),
                                                              owner(_owner),
                                                              explosionTime(_explosionTime) {}

Dynamite::~Dynamite() {}

void Dynamite::kick(sf::Vector2<int> _slideDirection) {
    slideSpeed = DYNAMITE_KICK_SPEED;
    slideDirection = sf::Vector2<float>(_slideDirection);
}

void Dynamite::update(Gamerules* gamerules) {
    if(slideSpeed != 0.0f) {
        position += slideDirection * DYNAMITE_KICK_DEACCELERATION;
        slideSpeed -= gamerules->getDeltaTime().asSeconds() * DYNAMITE_KICK_DEACCELERATION;
        slideSpeed = slideSpeed < 0.0f ? 0.0f : slideSpeed;
    }

    if(explosionTime < gamerules->getCurrentTime()) {
        explode(gamerules);
    }
}

void Dynamite::explode(Gamerules* gamerules) {
    std::vector<sf::Vector2<int>> dirs = {
        sf::Vector2<int>(0,-1),
        sf::Vector2<int>(0,1),
        sf::Vector2<int>(-1,0),
        sf::Vector2<int>(1,0)
    };
    sf::Vector2i center(position);

    gamerules->getVolatileEntitiesManager()->createFire(center, gamerules);
    for(auto& dir : dirs) {
        sf::Vector2i pos(center + dir);
        unsigned int i = 1;
        while( i < power && gamerules->getWorld()->getCell(pos) == WorldCell::GROUND) {
            pos += dir;
            gamerules->getVolatileEntitiesManager()->createFire(pos, gamerules);
            i++;
        }
        if(gamerules->getWorld()->getCell(pos) == WorldCell::BOX) {
            gamerules->getWorld()->changeCell(pos,WorldCell::GROUND,gamerules);
        }

        if(gamerules->getWorld()->getCell(pos) == WorldCell::GROUND && 
           gamerules->getVolatileEntitiesManager()->get(pos) == nullptr) {
             gamerules->getVolatileEntitiesManager()->createFire(pos, gamerules);
        }
    }
    destroy = true;
}