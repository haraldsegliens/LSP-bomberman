#include "../shared/dynamite.hpp"

CDynamite::CDynamite(sf::Vector2<float> _position,
	                 sf::Texture* _dynamiteTexture) : position(_position), 
												      dynamiteTexture(_dynamiteTexture) {}

CDynamite::~CDynamite() {}

void CDynamite::draw(sf::RenderWindow& window) {
	sf::RectangleShape rectangle;
    rectangle.setSize(sf::Vector2f(SPRITE_DYNAMITE_SIZE, SPRITE_DYNAMITE_SIZE));
    rectangle.setPosition(position.x * SPRITE_DYNAMITE_SIZE, 
                          position.y * SPRITE_DYNAMITE_SIZE - SPRITE_DYNAMITE_SIZE/2);
    rectangle.setTexture(dynamiteTexture);
    window.draw(rectangle);
}