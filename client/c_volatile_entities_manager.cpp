#include "../shared/volatile_entities_manager.hpp"
#include <iostream>

CVolatileEntitiesManager::CVolatileEntitiesManager() {
    //SPRITE_CELL_SIZE*SPRITE_CELL_SIZE * 6 sprite
    //uguns un 5 powerupi 1 rindā
    if(!tileMap.loadFromFile("materials/tile.png")) {
        std::cout << "Error loading player texture: " << "materials/tile.png" << std::endl;
    }
    volatileEntitiesMap = nullptr;
}

CVolatileEntitiesManager::~CVolatileEntitiesManager() {
    if(volatileEntitiesMap != nullptr) {
        delete [] volatileEntitiesMap;
    }
}

void CVolatileEntitiesManager::cleanup() {
    if(volatileEntitiesMap != nullptr) {
        clear();
    }
}

void CVolatileEntitiesManager::clear() {
    int i_max = width*height;
    for(int i = 0; i < i_max;i++) {
        VolatileEntity* entity = &volatileEntitiesMap[i];
        entity->type = VolatileEntityType::EMPTY;
        entity->powerupType = Powerup::NONE;
    }
}

void CVolatileEntitiesManager::load(Gamerules* gamerules) {
    cleanup();
    width = gamerules->getWorld()->getWidth();
    height = gamerules->getWorld()->getHeight();
    int i_max = width*height;
    volatileEntitiesMap = new VolatileEntity[i_max];
    for(int i = 0; i < i_max;i++) {
        VolatileEntity* entity = &volatileEntitiesMap[i];
        entity->type = VolatileEntityType::EMPTY;
        entity->powerupType = Powerup::NONE;
    }
}

void CVolatileEntitiesManager::draw(sf::RenderWindow& window) {
    for(int x = 0; x < width; x++) {
        for(int y = 0; y < height; y++) {
            VolatileEntity* entity = get(sf::Vector2<int>(x,y));
            if(entity == nullptr) {
                continue;
            }
            int i = -1;
            if(entity->type == VolatileEntityType::FIRE) {
                i = 0;
            } else if(entity->type == VolatileEntityType::POWERUP) {
                i = 1 + (int)entity->powerupType;
            } else {
                continue;
            }
            sf::RectangleShape rectangle;
            rectangle.setSize(sf::Vector2f(SPRITE_CELL_SIZE, SPRITE_CELL_SIZE));
            rectangle.setPosition((float)x * SPRITE_CELL_SIZE, 
                                  (float)y * SPRITE_CELL_SIZE);
            rectangle.setTexture(&tileMap);
            rectangle.setTextureRect(sf::IntRect(SPRITE_CELL_SIZE*i, 0, SPRITE_CELL_SIZE, SPRITE_CELL_SIZE));
            window.draw(rectangle);
        }
    }
}