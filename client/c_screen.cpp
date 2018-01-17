#include "c_screen.hpp"
#include <iostream>

CScreen::CScreen() : window(sf::VideoMode(800, 600), "Bomberman") {
    // setting frame limit to reduce GPU resources usage
    window.setFramerateLimit(60);
    std::cout << "Creating CScreen" << std::endl;
}

CScreen::~CScreen() {}

void CScreen::bindActionToShort(sf::Event::KeyEvent keyPressed, short& event) {
    switch (keyPressed.code) {
        case sf::Keyboard::Key::Up :
            event = event | (1 << UP);
        case sf::Keyboard::Key::Down:
            event = event | (1 << DOWN);
        case sf::Keyboard::Key::Left:
            event = event | (1 << LEFT);
        case sf::Keyboard::Key::Right:
            event = event | (1 << RIGHT);
        case sf::Keyboard::Key::Space:
            event = event | (1 << PLANT_BOMB);
        case sf::Keyboard::Key::R :
            event = event | (1 << DETONATE_REMOTELY);
        case sf::Keyboard::Key::E :
            event = event | (1 << KICK_DYNAMITE);
        // case sf::Keyboard::Key:: :
        //     action.set(7);
        // case sf::Keyboard::Key:: :
        //     action.set(8);
        case sf::Keyboard::Key::T :
            event = event | (1 << 9);
        case sf::Keyboard::Key::D :
            event = event | (1 << 10);
        case sf::Keyboard::Key::L :
            event = event | (1 << 11);
        case sf::Keyboard::Key::Num1 :
            event = event | (1 << 12);
        case sf::Keyboard::Key::Num2 :
            event = event | (1 << 13);
        case sf::Keyboard::Key::Num3 :
            event = event | (1 << 14);
        case sf::Keyboard::Key::Num4 :
            event = event | (1 << 15);
        default:
            break;
    }
}

WindowEvents CScreen::draw(Gamerules* gamerules) {
    WindowEvents events;
    events.inputState = 0;
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
                    // user is giving some input
                    bindActionToShort(event.key,events.inputState);
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
    return events;
}