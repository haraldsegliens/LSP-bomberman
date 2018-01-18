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
                VolatileEntity* up = _get(sf::Vector2<int>(x,y-1));
                VolatileEntity* left = _get(sf::Vector2<int>(x-1,y));
                VolatileEntity* right = _get(sf::Vector2<int>(x+1,y));
                VolatileEntity* down = _get(sf::Vector2<int>(x,y+1));
                if((up->isFire() && left->isFire()) ||
                    (up->isFire() && right->isFire()) ||
                    (down->isFire() && right->isFire()) ||
                    (down->isFire() && left->isFire())){
                    i = 1;
                } else if(left->isFire() && right->isFire()) {
                    i = 5;
                } else if(up->isFire() && down->isFire()) {
                    i = 6;
                } else if(right->isFire()) {
                    i = 0;
                } else if(left->isFire()) {
                    i = 2;
                } else if(up->isFire()) {
                    i = 3;
                } else if(down->isFire()) {
                    i = 4;
                }
            } else if(entity->type == VolatileEntityType::POWERUP) {
                i = 7 + (int)entity->powerupType;
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