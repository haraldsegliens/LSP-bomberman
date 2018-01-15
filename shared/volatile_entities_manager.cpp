#include "volatile_entities_manager.hpp"

VolatileEntity* VolatileEntitiesManager::get(sf::Vector2<int> pos) {
    return &volatileEntitiesMap[pos.y * gamerules->getWorld()->getWidth() + pos.x];
}

std::vector<sf::Vector2i> VolatileEntitiesManager::getAllPositionsWithType(VolatileEntityType type) {
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