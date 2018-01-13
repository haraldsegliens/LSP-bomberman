#ifndef C_SCREEN_H
#define C_SCREEN_H

#include <RenderWindow.hpp>

#include "../shared/gamerules.hpp"

struct WindowEvents {
	short inputState;
	bool windowClosed;
};

class CScreen {
	sf::RenderWindow window;
public:
	CScreen();
	~CScreen();
	WindowEvents draw(Gamerules* gamerules);
};

#endif
