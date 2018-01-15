#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include "shared_enums.hpp"

#ifdef CLIENT
#define World CWorld
#endif

class World;

#include "gamerules.hpp"

#ifdef SERVER
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
    sf::Vector2<int> closingPosition;
    sf::Time lastCloseTime;
    std::vector<WorldChange> changes;
    std::vector<sf::Vector2<int>> playerSpawnPoints;
#endif
public:

    World();
    ~World();

    int getWidth() { return mapSize.x; }
    int getHeight() { return mapSize.y; }
    WorldCell getCell(int i) { return map[i]; }
    WorldCell getCell(sf::Vector2<int> pos) { return map[pos.x + pos.y * mapSize.x]; }
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

    sf::Vector2<int> getSpawnPoint(int i) {
        return playerSpawnPoints[i];
    }
    std::vector<WorldChange> popChanges() {
        std::vector<WorldChange> popChanges = changes;
        changes.clear();
        return popChanges;
    }
#endif
};
