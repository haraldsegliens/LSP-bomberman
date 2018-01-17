#ifndef C_SCREEN_H
#define C_SCREEN_H

#include <SFML/Graphics.hpp>

class CScreen;

#include "../shared/gamerules.hpp"

struct WindowEvents {
    short inputState;
    bool windowClosed;
};

class CScreen {
    sf::RenderWindow window;

    void bindActionToShort(sf::Event::KeyEvent keyPressed, short& event);
public:
    CScreen();
    ~CScreen();
    WindowEvents draw(Gamerules* gamerules);
};

#endif
