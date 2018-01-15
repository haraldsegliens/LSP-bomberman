#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <Vector2.hpp>
#include "shared_enums.hpp"

#ifdef CLIENT
#include <RenderWindow.hpp>
#include <Texture.hpp>

#define World CWorld
#else
#include <Time.hpp>
#include "gamerules.hpp"

struct WorldChange {
    sf::Vector2<int> position;
    WorldCell value;
};
#endif

class World {
    sf::Vector2<int> mapSize;
    std::vector<WorldCell> map;//taja pašā kārtībā kā bija GameStart paketē
#ifdef CLIENT
    sf::Texture tileset;
#else
    bool closing;
    int closingPathIndex;
    std::vector<sf::Vector2<int>> closingPath;
    sf::Vector2<int> closingPosition
    sf::Time lastCloseTime;
    std::vector<WorldChange> changes;
    std::vector<sf::Vector2<int>> playerSpawnPoints;
#endif
public:

    World();
    ~World();

    int getWidth() { return mapSize.x; }
    int getHeight() { return mapSize.y; }
    WorldCell getCell(sf::Vector2<int> pos) { return mapSize[pos.x + pos.y * mapSize.x]; }
    void cleanup();
#ifdef CLIENT
    void draw(sf::RenderWindow& window);
    void loadMap(std::vector<WorldCell>& _map,sf::Vector2<int> _mapSize);
    void changeCell(sf::Vector2<int> pos, WorldCell value);
#else
    void update(Gamerules* gamerules);
    void loadMap(std::vector<WorldCell>& _map,sf::Vector2<int> _mapSize,std::vector<sf::Vector2<int>> _playerSpawnPoints);
    void startClosing() { closing = true; }
    bool isClosing() { return closing; }
    void changeCell(sf::Vector2<int> pos, WorldCell value, Gamerules* gamerules);
#endif
};

#endif
