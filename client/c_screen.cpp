#include "c_screen.hpp"
#include <iostream>

CScreen::CScreen() : window(sf::VideoMode(800, 600), "Bomberman"), inputState(0) {
    // setting frame limit to reduce GPU resources usage
    window.setFramerateLimit(60);
    std::cout << "Creating CScreen" << std::endl;
}

CScreen::~CScreen() {}

int CScreen::getPlayerBitFromEvent(sf::Event::KeyEvent key) {
    switch (key.code) {
        case sf::Keyboard::Key::Up :
            return UP;
        case sf::Keyboard::Key::Down:
            return DOWN;
        case sf::Keyboard::Key::Left:
            return LEFT;
        case sf::Keyboard::Key::Right:
            return RIGHT;
        case sf::Keyboard::Key::Space:
            return PLANT_BOMB;
        case sf::Keyboard::Key::R :
            return DETONATE_REMOTELY;
        case sf::Keyboard::Key::E :
            return KICK_DYNAMITE;
        // case sf::Keyboard::Key:: :
        //     action.set(7);
        // case sf::Keyboard::Key:: :
        //     action.set(8);
        case sf::Keyboard::Key::T :
            return 9;
        case sf::Keyboard::Key::D :
            return 10;
        case sf::Keyboard::Key::L :
            return 11;
        case sf::Keyboard::Key::Num1 :
            return 12;
        case sf::Keyboard::Key::Num2 :
            return 13;
        case sf::Keyboard::Key::Num3 :
            return 14;
        case sf::Keyboard::Key::Num4 :
            return 15;
        default:
            return -1;
            break;
    }
}

void CScreen::bindActionToShort(sf::Event::KeyEvent keyPressed, unsigned short& event) {
    int bit = getPlayerBitFromEvent(keyPressed);
    if(bit != -1) {
        event |= (1 << bit);
    }
}

void CScreen::unbindActionToShort(sf::Event::KeyEvent keyReleased, unsigned short& event) {
    int bit = getPlayerBitFromEvent(keyReleased);
    if(bit != -1) {
        event &= ~(1 << bit);
    }
}

WindowEvents CScreen::draw(Gamerules* gamerules) {
    WindowEvents events;
    if(window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                // window closed
                case sf::Event::Closed:
                    events.windowClosed = true;
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    bindActionToShort(event.key,inputState);
                    break;
                case sf::Event::KeyReleased:
                    unbindActionToShort(event.key,inputState);
                    break;
                default:
                    break;
            }
        }
        sf::View view(sf::FloatRect(0, 0, gamerules->getWorld()->getWidth()*SPRITE_CELL_SIZE, gamerules->getWorld()->getHeight()*SPRITE_CELL_SIZE));
        window.setView(view);
        window.clear(sf::Color::Black);
        gamerules->draw(window);
        window.display();
        events.windowClosed = false;
    } else {
        events.windowClosed = true;
    }
    events.inputState = inputState;
    return events;
}