#include "c_screen.hpp"
#include <iostream>
#include <bitset>

CScreen::CScreen() : window(sf::VideoMode(800, 600), "Bomberman") {
    std::cout << "Creating CScreen" << std::endl;
}

CScreen::~CScreen() {}

short CScreen::bindActionToShort(sf::Event::KeyEvent keyPressed) {
    std::bitset<16> action;
    action.reset();
    switch (keyPressed.code) {
        case sf::Keyboard::Key::Up :
            action.set(0);
        case sf::Keyboard::Key::Down:
            action.set(1);
        case sf::Keyboard::Key::Left:
            action.set(2);
        case sf::Keyboard::Key::Right:
            action.set(3);
        case sf::Keyboard::Key::Space:
            action.set(4);
        case sf::Keyboard::Key::R :
            action.set(5);
        case sf::Keyboard::Key::E :
            action.set(6);
        // case sf::Keyboard::Key:: :
        //     action.set(7);
        // case sf::Keyboard::Key:: :
        //     action.set(8);
        case sf::Keyboard::Key::T :
            action.set(9);
        case sf::Keyboard::Key::D :
            action.set(10);
        case sf::Keyboard::Key::L :
            action.set(11);
        case sf::Keyboard::Key::Num1 :
            action.set(12);
        case sf::Keyboard::Key::Num2 :
            action.set(13);
        case sf::Keyboard::Key::Num3 :
            action.set(14);
        case sf::Keyboard::Key::Num4 :
            action.set(15);
        default:
            break;
    }
    return (short)(action.to_ulong());
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
                    // user is giving some input
                    events.inputState = bindActionToShort(event.key);
                    break;
                default:
                    break;
            }
        }

        window.clear(sf::Color::Black);
        gamerules->draw(window);
        window.display();
        events.windowClosed = false;
    } else {
        events.windowClosed = true;
    }
    return events;
}