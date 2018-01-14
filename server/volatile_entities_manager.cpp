#include "../shared/volatile_entities_manager.hpp"

VolatileEntitiesManager::VolatileEntitiesManager(Gamerules* _gamerules) : gamerules(_gamerules) {
    cleanup();
}

VolatileEntitiesManager::~VolatileEntitiesManager() {}

void VolatileEntitiesManager::cleanup() {
    volatileEntitiesMap.clear();
    int i_max = gamerules->getWorld()->getWidth()*gamerules->getWorld()->getHeight();
    for(int i = 0; i < i_max;i++) {
        VolatileEntity entity;
        entity.type = VolatileEntityType::EMPTY;
        entity.powerupType = Powerup::NONE;
        volatileEntitiesMap.push_back(entity);
    }
}

void VolatileEntitiesManager::update(Gamerules* gamerules) {
    auto it = events.begin();
    while(it != events.end() && *it.time <= gamerules->getCurrentTime()) {
        entity->type = VolatileEntityType::EMPTY;
        entity->powerupType = Powerup::NONE;
        it = events.erase(it);
    }
}

void VolatileEntitiesManager::deleteEntity(VolatileEntity* entity) {
    entity->type = VolatileEntityType::EMPTY;
    entity->powerupType = Powerup::NONE;
    events.erase(event);
}

VolatileEntity* VolatileEntitiesManager::createFire(sf::Vector2<int> pos) {
    VolatileEntity* entity = get(pos);
    entity->type = VolatileEntityType::FIRE;
    entity->event = addEvent(entity,gamerules->getCurrentTime() + sf::seconds(FIRE_DURATION));
    return entity;
}

VolatileEntity* VolatileEntitiesManager::createPowerup(sf::Vector2<int> pos, Powerup powerupType) {
    VolatileEntity* entity = get(pos);
    entity->type = VolatileEntityType::FIRE;
    entity->powerupType = powerupType;
    entity->event = addEvent(entity,gamerules->getCurrentTime() + sf::seconds(POWERUP_ALIVE_DURATION));
    return entity;
}

std::list<VolatileEntityEvent>::iterator addEvent(VolatileEntity* entity, sf::Time time) {
    VolatileEntityEvent event;
    event.time = time;
    event.entity = entity;
    auto it = events.begin();
    while(it != events.end() && *it->time <= time) { ++it; }
    return events.insert(it,event);
}