#include "../shared/volatile_entities_manager.hpp"

VolatileEntitiesManager::VolatileEntitiesManager() {
    cleanup();
}

VolatileEntitiesManager::~VolatileEntitiesManager() {}

void VolatileEntitiesManager::load(Gamerules* gamerules) {
    cleanup();
    volatileEntitiesMap.clear();
    width = gamerules->getWorld()->getWidth();
    height = gamerules->getWorld()->getHeight();
    int i_max = width*height;
    for(int i = 0; i < i_max;i++) {
        VolatileEntity entity;
        entity.type = VolatileEntityType::EMPTY;
        entity.powerupType = Powerup::NONE;
        volatileEntitiesMap.push_back(entity);
    }
}

void VolatileEntitiesManager::cleanup() {
    volatileEntitiesMap.clear();
    events.clear();
}

void VolatileEntitiesManager::update(Gamerules* gamerules) {
    auto it = events.begin();
    while(it != events.end() && it->time <= gamerules->getCurrentTime()) {
        it->entity->type = VolatileEntityType::EMPTY;
        it->entity->powerupType = Powerup::NONE;
        it = events.erase(it);
    }
}

void VolatileEntitiesManager::deleteEntity(VolatileEntity* entity) {
    entity->type = VolatileEntityType::EMPTY;
    entity->powerupType = Powerup::NONE;
    events.erase(entity->event);
}

VolatileEntity* VolatileEntitiesManager::createFire(sf::Vector2<int> pos, Gamerules* gamerules) {
    VolatileEntity* entity = get(pos);
    if(entity->type != VolatileEntityType::EMPTY) {
        deleteEntity(entity);
    }
    entity->type = VolatileEntityType::FIRE;
    entity->event = addEvent(entity,gamerules->getCurrentTime() + sf::seconds(FIRE_DURATION));
    return entity;
}

VolatileEntity* VolatileEntitiesManager::createPowerup(sf::Vector2<int> pos, Powerup powerupType, Gamerules* gamerules) {
    VolatileEntity* entity = get(pos);
    entity->type = VolatileEntityType::POWERUP;
    entity->powerupType = powerupType;
    entity->event = addEvent(entity,gamerules->getCurrentTime() + sf::seconds(POWERUP_ALIVE_DURATION));
    return entity;
}

std::list<VolatileEntityEvent>::iterator VolatileEntitiesManager::addEvent(VolatileEntity* entity, sf::Time time) {
    VolatileEntityEvent event;
    event.time = time;
    event.entity = entity;
    auto it = events.begin();
    while(it != events.end() && it->time <= time) { ++it; }
    return events.insert(it,event);
}