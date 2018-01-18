#include "volatile_entities_manager.hpp"

VolatileEntity* VolatileEntitiesManager::_get(sf::Vector2<int> pos) {
    return &volatileEntitiesMap[pos.y * width + pos.x];
}

VolatileEntity* VolatileEntitiesManager::get(sf::Vector2<int> pos) {
    VolatileEntity* entity = _get(pos);
    if(entity->type == VolatileEntityType::EMPTY) {
        return nullptr;
    } else {
        return entity;
    }
}

std::vector<sf::Vector2i> VolatileEntitiesManager::getAllPositionsWithType(VolatileEntityType type) {
    std::vector<sf::Vector2i> result;
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            sf::Vector2<int> pos(x,y);
            VolatileEntity* entity = get(pos);
            if(entity != nullptr && entity->type == type) {
                result.push_back(pos);
            }
        }
    }
    return result;
}