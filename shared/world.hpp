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
    std::vector<WorldCell> map;//taja pašā kārtībā kā bija GameStart paketē
#ifdef CLIENT
    sf::Texture tileset;
#else
    bool isClosing;
    int closingPathIndex;
    std::vector<sf::Vector2<int>> closingPath;
    sf::Vector2<int> closingPosition
    sf::Time lastCloseTime;
    std::vector<WorldChange> changes;
#endif
public:
    World();
    ~World();
#ifdef CLIENT
    void draw(sf::RenderWindow& window);
#else
    void updateKeepAlive();
    void update();
    void destroy();
#endif
};

#endif
