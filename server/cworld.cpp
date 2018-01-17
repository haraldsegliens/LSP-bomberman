#include "../shared/world.hpp"
#include <stdlib.h>
#include <time.h>

World::World() {
    cleanup();
    srand (time(NULL));
}
World::~World() {}


void World::cleanup() {
    sf::Vector2i _mapSize(13,13);
    std::vector<int> _map = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };
    std::vector<sf::Vector2<int>> _playerSpawnPoints = {
        sf::Vector2<int>(1,1),sf::Vector2<int>(11,1),sf::Vector2<int>(1,11),sf::Vector2<int>(11,11)
    };

    std::vector<WorldCell> castedMap;
    for(int a : _map) {
        castedMap.push_back((WorldCell)a);
    }
    loadMap(castedMap,_mapSize,_playerSpawnPoints);
}

void World::update(Gamerules* gamerules) {}

void World::loadMap(std::vector<WorldCell>& _map,
                    sf::Vector2<int> _mapSize,
                    std::vector<sf::Vector2<int>> _playerSpawnPoints) {
    this->map = _map;
    this->mapSize = _mapSize;
    this->playerSpawnPoints = _playerSpawnPoints;
}

void World::changeCell(sf::Vector2<int> pos, WorldCell value, Gamerules* gamerules) {
    if(getCell(pos) == WorldCell::BOX && value == WorldCell::GROUND && (rand()%3) == 0) {
        gamerules->getVolatileEntitiesManager()->createPowerup(pos,(Powerup)(rand() % 5), gamerules);
    }
    map[pos.x + pos.y * mapSize.x] = value;
    WorldChange change;
    change.position = pos;
    change.value = value;
    changes.push_back(change);
}
