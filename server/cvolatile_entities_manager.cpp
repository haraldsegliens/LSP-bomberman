#include "../shared/volatile_entities_manager.hpp"

VolatileEntitiesManager::VolatileEntitiesManager() {
    cleanup();
}

VolatileEntitiesManager::~VolatileEntitiesManager() {}

void VolatileEntitiesManager::load(Gamerules* _gamerules) {
    cleanup();
    gamerules = _gamerules;
    volatileEntitiesMap.clear();
    int i_max = gamerules->getWorld()->getWidth()*gamerules->getWorld()->getHeight();
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

void VolatileEntitiesManager::update(Gamerules* _gamerules) {
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

std::list<VolatileEntityEvent>::iterator VolatileEntitiesManager::addEvent(VolatileEntity* entity, sf::Time time) {
    VolatileEntityEvent event;
    event.time = time;
    event.entity = entity;
    auto it = events.begin();
    while(it != events.end() && it->time <= time) { ++it; }
    return events.insert(it,event);
}