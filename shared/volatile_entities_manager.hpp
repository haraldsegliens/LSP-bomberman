#ifndef VOLATILE_ENTITIES_MANAGER_H
#define VOLATILE_ENTITIES_MANAGER_H

#include <vector>
#include <list>
#include <SFML/Graphics.hpp>
#include "shared_enums.hpp"

#ifdef CLIENT
#define VolatileEntitiesManager CVolatileEntitiesManager
#endif

class VolatileEntitiesManager;

typedef enum { 
    EMPTY = 0,
    FIRE = 1,
    POWERUP = 2
} VolatileEntityType;

#ifdef SERVER
struct VolatileEntity;
struct VolatileEntityEvent {
    sf::Time time;
    VolatileEntity* entity;
};
#endif

#include "gamerules.hpp"

struct VolatileEntity {
    VolatileEntityType type;
    Powerup powerupType;

#ifdef SERVER
    std::list<VolatileEntityEvent>::iterator event;
#endif
};

class VolatileEntitiesManager {
    int width;
    int height;
    VolatileEntity* volatileEntitiesMap;//tādā pašā kārtībā, kā World šūnas
#ifdef CLIENT
    sf::Texture tileMap;
#else
    std::list<VolatileEntityEvent> events;
#endif
public:
    VolatileEntitiesManager();
    ~VolatileEntitiesManager();
    void load(Gamerules* _gamerules);
    VolatileEntity* get(sf::Vector2<int> pos);
    VolatileEntity* _get(sf::Vector2<int> pos);
    void cleanup();
    std::vector<sf::Vector2i> getAllPositionsWithType(VolatileEntityType type);

#ifdef CLIENT
    void draw(sf::RenderWindow& window);
    void clear();
#else
    void update(Gamerules* gamerules);
    void deleteEntity(VolatileEntity* entity);
    VolatileEntity* createFire(sf::Vector2<int> pos, Gamerules* gamerules);
    VolatileEntity* createPowerup(sf::Vector2<int> pos, Powerup powerupType, Gamerules* gamerules);
    std::list<VolatileEntityEvent>::iterator addEvent(VolatileEntity* entity, sf::Time time);
#endif
};

#endif