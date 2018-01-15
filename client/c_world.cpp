#include "../shared/world.hpp"

CWorld::CWorld(std::vector<WorldCell>& _map,sf::Vector2<int> _mapSize) {
    //spritesheet: ar SPRITE_CELL_SIZE*SPRITE_CELL_SIZE * 3 bildēm: zeme, kaste, siena
    if(!tileset.loadFromFile("materials/world.png")) {
        std::cout << "Error loading world texture: " << "materials/world.png" << std::endl;
    }
    loadMap(_map,_mapSize);
}

~CWorld::CWorld() {}

void CWorld::cleanup() {}

void CWorld::draw(sf::RenderWindow& window) {
    for(int x = 0; x < getWidth(); x++) {
        for(int y = 0; y < getHeight(); y++) {
            WorldCell cell = get(sf::Vector2<int>(x,y));
            sf::RectangleShape rectangle;
            rectangle.setSize(sf::Vector2f(SPRITE_CELL_SIZE, SPRITE_CELL_SIZE));
            rectangle.setPosition(((float)x + 0.5f) * SPRITE_CELL_SIZE, 
                                  ((float)y + 0.5f) * SPRITE_CELL_SIZE);
            rectangle.setTexture(&tileMap);
            rectangle.setTextureRect(sf::IntRect(SPRITE_CELL_SIZE*((int)cell), 0, SPRITE_CELL_SIZE, SPRITE_CELL_SIZE));
            window.draw(rectangle);
        }
    }
}

void CWorld::loadMap(std::vector<WorldCell>& _map,sf::Vector2<int> _mapSize) {
    mapSize = _mapSize;
    map = _map;
}