#ifndef C_SCREEN_H
#define C_SCREEN_H

#include <SFML/RenderWindow.hpp>

#include "../shared/gamerules.hpp"

struct WindowEvents {
    short inputState;
    bool windowClosed;
};

class CScreen {
    sf::RenderWindow window;

    short bindActionToShort(sf::Event::KeyEvent keyPressed)
public:
    CScreen();
    ~CScreen();
    WindowEvents draw(Gamerules* gamerules);
};

#endif
