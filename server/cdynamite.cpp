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

bool Dynamite::move(sf::Vector2f targetPosition, Gamerules* gamerules) {
    SurroundingInfo currentInfo = gamerules->scanSurrounding(position,PLAYER_CHECKBOX_SIZE_RELATIVE);
    SurroundingInfo targetInfo = gamerules->scanSurrounding(targetPosition,PLAYER_CHECKBOX_SIZE_RELATIVE);
    if(targetInfo.containsWorldCell(WorldCell::WALL) || targetInfo.containsWorldCell(WorldCell::BOX)) {
        return false;
    }

    if(targetInfo.players.size() >= 1 && currentInfo.players.size() == 0) {
        return false;
    }

    if(targetInfo.dynamites.size() == 2) {
        return false;
    }
    position = targetPosition;
    return true;
}

void Dynamite::update(Gamerules* gamerules) {
    if(slideSpeed != 0.0f) {
        if(move(position + slideDirection * slideSpeed * gamerules->getDeltaTime().asSeconds(),gamerules)){
            slideSpeed -= gamerules->getDeltaTime().asSeconds() * DYNAMITE_KICK_DEACCELERATION;
            slideSpeed = slideSpeed < 0.0f ? 0.0f : slideSpeed;
        } else {
            slideSpeed = 0.0f;
        }
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
            gamerules->getVolatileEntitiesManager()->createFire(pos, gamerules);
            pos += dir;
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