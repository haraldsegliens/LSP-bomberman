#ifndef C_SCREEN_H
#define C_SCREEN_H

#include <SFML/Graphics.hpp>

class CScreen;

#include "../shared/gamerules.hpp"

struct WindowEvents {
    unsigned short inputState;
    bool windowClosed;
};

class CScreen {
    sf::RenderWindow window;
    unsigned short inputState;
    int getPlayerBitFromEvent(sf::Event::KeyEvent key);
    void bindActionToShort(sf::Event::KeyEvent keyPressed, unsigned short& event);
    void unbindActionToShort(sf::Event::KeyEvent keyPressed, unsigned short& event);
public:
    CScreen();
    ~CScreen();
    WindowEvents draw(Gamerules* gamerules);
};

#endif
