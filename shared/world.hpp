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

    void loadMap(std::vector<WorldCell>& map,sf::Vector2<int> mapSize,std::vector<sf::Vector2<int>> playerSpawnPoints);
    int getWidth() { return mapSize.x; }
    int getHeight() { return mapSize.y; }
    WorldCell getCell(sf::Vector2<int> pos) { return mapSize[pos.x + pos.y * mapSize.y]; }
    void cleanup();
    void changeCell(sf::Vector2<int> pos, WorldCell value);
#ifdef CLIENT
    void draw(sf::RenderWindow& window);
#else
    void updateKeepAlive();
    void update(Gamerules* gamerules);
    void startClosing() { closing = true; }
    bool isClosing() { return closing; }
#endif
};

#endif
