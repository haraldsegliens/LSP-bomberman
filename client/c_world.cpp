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
    sf::RenderStates states;
    states.texture = &tileset;
    window.draw(vertices, states);
}

int CWorld::getSpriteId(sf::Vector2i pos) {
    int thisCell = getCell(pos);
    int upperCell = getCell(pos + sf::Vector2i(0,-1));
    if(upperCell == WorldCell::WALL && thisCell == WorldCell::GROUND)
    {
        return 1;
    }
    if(thisCell == WorldCell::GROUND)
    {
        return 0;
    }

    return thisCell+1;
}

void CWorld::loadMap(std::vector<WorldCell>& _map,sf::Vector2<int> _mapSize) {
    mapSize = _mapSize;
    map = _map;

    vertices.setPrimitiveType(sf::Quads);
    vertices.resize(mapSize.x * mapSize.y * 4);
    for(int x = 0; x < getWidth(); x++) {
        for(int y = 0; y < getHeight(); y++) {
            sf::Vertex* quad = &vertices[(x + y * getWidth()) * 4];
            int cell = getSpriteId(sf::Vector2<int>(x,y));

            // define its 4 corners
            quad[0].position = sf::Vector2f(x * SPRITE_CELL_SIZE, y * SPRITE_CELL_SIZE);
            quad[1].position = sf::Vector2f((x + 1) * SPRITE_CELL_SIZE, y * SPRITE_CELL_SIZE);
            quad[2].position = sf::Vector2f((x + 1) * SPRITE_CELL_SIZE, (y + 1) * SPRITE_CELL_SIZE);
            quad[3].position = sf::Vector2f(x * SPRITE_CELL_SIZE, (y + 1) * SPRITE_CELL_SIZE);

            quad[0].texCoords = sf::Vector2f(cell * SPRITE_CELL_SIZE, 0);
            quad[1].texCoords = sf::Vector2f((cell + 1) * SPRITE_CELL_SIZE, 0);
            quad[2].texCoords = sf::Vector2f((cell + 1) * SPRITE_CELL_SIZE, SPRITE_CELL_SIZE);
            quad[3].texCoords = sf::Vector2f(cell * SPRITE_CELL_SIZE, SPRITE_CELL_SIZE);
        }
    }
}

void CWorld::changeCell(sf::Vector2<int> pos, WorldCell value) {
    int i = pos.x + pos.y * mapSize.x;
    map[i] = value;
    sf::Vertex* quad = &vertices[i * 4];
    int cell = (int)value;
    quad[0].texCoords = sf::Vector2f(cell * SPRITE_CELL_SIZE, 0);
    quad[1].texCoords = sf::Vector2f((cell + 1) * SPRITE_CELL_SIZE, 0);
    quad[2].texCoords = sf::Vector2f((cell + 1) * SPRITE_CELL_SIZE, SPRITE_CELL_SIZE);
    quad[3].texCoords = sf::Vector2f(cell * SPRITE_CELL_SIZE, SPRITE_CELL_SIZE);
}