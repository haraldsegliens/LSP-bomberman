#include "../shared/world.hpp"
#include <stdlib.h>
#include <time.h>

World::World() {
    cleanup();
    srand (time(NULL));
}
World::~World() {}


void World::cleanup() {
    std::vector<int> _mapSize = {13,13};
    std::vector<int> _map = {
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
        2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,
        2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
        2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,
        2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
        2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,
        2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
        2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,
        2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
        2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,
        2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
    };
    std::vector<sf::Vector2<int>> _playerSpawnPoints = {
        sf::Vector2<int>(),sf::Vector2<int>()
    };
    loadMap(_map,_mapSize,_playerSpawnPoints);
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
        gamerules->getVolatileEntitiesManager()->createPowerup(pos,(Powerup)(rand() % 5));
    }
    mapSize[pos.x + pos.y * mapSize.x] = value;
    WorldChange change;
    change.position = pos;
    change.value = value;
    changes.push_back(change);
}
