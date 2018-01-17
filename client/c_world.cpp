#include <iostream>
#include "../shared/world.hpp"

CWorld::CWorld() {
    //spritesheet: ar SPRITE_CELL_SIZE*SPRITE_CELL_SIZE * 3 bildēm: zeme, kaste, siena
    if(!tileset.loadFromFile("materials/world.png")) {
        std::cout << "Error loading world texture: " << "materials/world.png" << std::endl;
    }
}

CWorld::~CWorld() {}

void CWorld::cleanup() {}

void CWorld::draw(sf::RenderWindow& window) {
    sf::View view(sf::FloatRect(0, 0, getWidth()*SPRITE_CELL_SIZE, getHeight()*SPRITE_CELL_SIZE));
    window.setView(view);
    for(int x = 0; x < getWidth(); x++) {
        for(int y = 0; y < getHeight(); y++) {
            WorldCell cell = getCell(sf::Vector2<int>(x,y));
            sf::RectangleShape rectangle;
            rectangle.setSize(sf::Vector2f(SPRITE_CELL_SIZE, SPRITE_CELL_SIZE));
            rectangle.setPosition(((float)x) * SPRITE_CELL_SIZE, 
                                  ((float)y) * SPRITE_CELL_SIZE);
            rectangle.setTexture(&tileset);
            rectangle.setTextureRect(sf::IntRect(SPRITE_CELL_SIZE*((int)cell), 0, SPRITE_CELL_SIZE, SPRITE_CELL_SIZE));
            window.draw(rectangle);
        }
    }
}

void CWorld::loadMap(std::vector<WorldCell>& _map,sf::Vector2<int> _mapSize) {
    mapSize = _mapSize;
    map = _map;
}

void CWorld::changeCell(sf::Vector2<int> pos, WorldCell value) {
    map[pos.x + pos.y * mapSize.x] = value;
}