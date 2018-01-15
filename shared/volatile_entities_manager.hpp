#ifndef VOLATILE_ENTITIES_MANAGER_H
#define VOLATILE_ENTITIES_MANAGER_H

#include "shared_enums.hpp"
#include <vector>
#include <list>
#include <Vector2.hpp>
#include <Time.hpp>

#ifdef CLIENT
#include <RenderWindow.hpp>
#include <Texture.hpp>
#define VolatileEntitiesManager CVolatileEntitiesManager
#else
struct VolatileEntity;
struct VolatileEntityEvent {
    sf:Time time;
    VolatileEntity* entity;
};
#endif

typedef enum { 
    EMPTY = 0,
    FIRE = 1,
    POWERUP = 2
} VolatileEntityType;

struct VolatileEntity {
    VolatileEntityType type;
    Powerup powerupType;

#ifdef SERVER
    std::list<VolatileEntityEvent>::iterator event;
#endif
};

class VolatileEntitiesManager {
    std::vector<VolatileEntity> volatileEntitiesMap;//tādā pašā kārtībā, kā World šūnas
    Gamerules* gamerules;
#ifdef CLIENT
    sf::Texture tileMap;
#else
    std::list<VolatileEntityEvent> events;
#endif
public:
    VolatileEntitiesManager();
    ~VolatileEntitiesManager();
    void load(Gamerules* _gamerules);
    VolatileEntity* get(sf::Vector2<int> pos) {
        return &volatileEntitiesMap[pos.y * gamerules->getWorld()->getWidth() + pos.x];
    }
    void cleanup();

    std::vector<sf::Vector2i> getAllPositionsWithType(VolatileEntityType type) {
        std::vector<sf::Vector2i> result;
        for(int y = 0; y < gamerules->getWorld()->getHeight(); y++) {
            for(int x = 0; x < gamerules->getWorld()->getWidth(); x++) {
                sf::Vector2<int> pos(x,y);
                VolatileEntity* entity = get(pos);
                if(entity->type == type) {
                    result.push_back(pos);
                }
            }
        }
        return result;
    }

#ifdef CLIENT
    void draw(sf::RenderWindow& window);
    void clear();
#else
    void update(Gamerules* _gamerules);
    void deleteEntity(VolatileEntity* entity);
    VolatileEntity* createFire(sf::Vector2<int> pos);
    VolatileEntity* createPowerup(sf::Vector2<int> pos, Powerup powerupType);
    std::list<VolatileEntityEvent>::iterator addEvent(VolatileEntity* entity, sf::Time time);
#endif
};

#endif