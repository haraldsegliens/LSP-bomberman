#include "../shared/player.hpp"
#include <iostream>

CPlayer::CPlayer(int _id,std::string _name,
                 sf::Vector2<float> _position, 
                 sf::Vector2<int> _direction, int order) : id(_id), name(_name),
                                               position(_position),
                                               direction(_direction),
                                               powerup(Powerup::NONE),
                                               power(1), speed(1),
                                               maxDynamiteCount(1) {
    //SPRITE_PLAYER_SIZE*SPRITE_PLAYER_SIZE sprite * 4 = spritesheet
    //up,down,left,right
    if(!playerTexture.loadFromFile("materials/player_" + std::to_string(order) + ".png")) {
        std::cout << "Error loading player texture: " << "materials/player_" + std::to_string(order) + ".png" << std::endl;
    }
}

CPlayer::~CPlayer() {}

void CPlayer::draw(sf::RenderWindow& window) {
    sf::Vector2f _position = getPosition();
    int i = direction.x == 0 ? (direction.y == -1 ? 0 : 1) : (direction.x == -1 ? 2 : 3);
    sf::RectangleShape rectangle;
    rectangle.setSize(sf::Vector2f(SPRITE_PLAYER_SIZE_WIDTH, SPRITE_PLAYER_SIZE_HEIGTH));
    rectangle.setPosition((_position.x-0.5f) * SPRITE_CELL_SIZE, 
                          (_position.y-0.5f) * SPRITE_CELL_SIZE - SPRITE_PLAYER_SIZE_HEIGTH/2);
    rectangle.setTexture(&playerTexture);
    rectangle.setTextureRect(sf::IntRect(SPRITE_PLAYER_SIZE_WIDTH*i, 0, SPRITE_PLAYER_SIZE_WIDTH, SPRITE_PLAYER_SIZE_HEIGTH));
    window.draw(rectangle);
}

sf::Vector2f CPlayer::getPosition() {
    return position;
}

void CPlayer::setPosition(sf::Vector2f a) {
    position = a;
}