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
    sf:Time createdTime;
    int dynamitePower;
    std::vector<std::list<VolatileEntityEvent>::iterator> events;
#endif
};

class VolatileEntitiesManager {
    Gamerules* gamerules;
    std::vector<VolatileEntity> volatileEntitiesMap;//tādā pašā kārtībā, kā World šūnas
#ifdef CLIENT
    sf::Texture tileMap;
#else
    std::list<VolatileEntityEvent> events;
#endif
public:
    VolatileEntitiesManager(Gamerules* gamerules);
    ~VolatileEntitiesManager();

    VolatileEntity* get(sf::Vector2<int> pos) {
        return &volatileEntitiesMap[pos.y * gamerules->getWorld()->getWidth() + pos.x];
    }
    void cleanup();

#ifdef CLIENT
    void draw(sf::RenderWindow& window);
#else
    void update(Gamerules* gamerules);
    void deleteEntity(VolatileEntity* entity);
    void addEvent(VolatileEntity* entity, sf::Time time);
    VolatileEntity* createFire(sf::Vector2<int> pos);
    VolatileEntity* createPowerup(sf::Vector2<int> pos, Powerup powerupType);
#endif
};

#endif