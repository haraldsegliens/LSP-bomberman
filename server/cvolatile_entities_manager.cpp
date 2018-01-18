#include "../shared/volatile_entities_manager.hpp"
#include <iostream>

VolatileEntitiesManager::VolatileEntitiesManager() {
    volatileEntitiesMap = nullptr;
    cleanup();
}

VolatileEntitiesManager::~VolatileEntitiesManager() {}

void VolatileEntitiesManager::load(Gamerules* gamerules) {
    cleanup();
    width = gamerules->getWorld()->getWidth();
    height = gamerules->getWorld()->getHeight();
    volatileEntitiesMap = new VolatileEntity[width*height];
    int i_max = width*height;
    for(int i = 0; i < i_max;i++) {
        VolatileEntity* entity = &volatileEntitiesMap[i];
        entity->type = VolatileEntityType::EMPTY;
        entity->powerupType = Powerup::NONE;
    }
}

void VolatileEntitiesManager::cleanup() {
    if(volatileEntitiesMap != nullptr) {
        delete [] volatileEntitiesMap;
        volatileEntitiesMap = nullptr;
    }
    events.clear();
}

void VolatileEntitiesManager::update(Gamerules* gamerules) {
    auto it = events.begin();
    while(it != events.end() && it->time <= gamerules->getCurrentTime()) {
        std::cout << "VolatileEntitiesManager erase" << std::endl;
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
    std::cout << "Create fire " << pos.x << " " << pos.y << std::endl;
    VolatileEntity* entity = _get(pos);
    if(entity->type != VolatileEntityType::EMPTY) {
        deleteEntity(entity);
    }
    entity->type = VolatileEntityType::FIRE;
    entity->event = addEvent(entity,gamerules->getCurrentTime() + sf::seconds(FIRE_DURATION));
    return entity;
}

VolatileEntity* VolatileEntitiesManager::createPowerup(sf::Vector2<int> pos, Powerup powerupType, Gamerules* gamerules) {
    VolatileEntity* entity = _get(pos);
    if(entity->type != VolatileEntityType::EMPTY) {
        deleteEntity(entity);
    }
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