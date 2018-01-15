#include "c_screen.hpp"
#include <iostream>

CScreen::CScreen() : window(sf::VideoMode(800, 600), "Bomberman") {
    std::cout << "Creating CScreen" << std::endl;
}

CScreen::~CScreen() {

}

short CScreen::bindActionToShort(sf::Event::KeyEvent keyPressed) {
    bitset<16> action;
    action.reset();
    sf::Keyboard keyboard;
    switch (keyPressed.code)
    {
        case keyboard.Up :
            action.set(0);
        case keyboard.Down:
            action.set(1);
        case keyboard.Left:
            action.set(2);
        case keyboard.Right:
            action.set(3);
        case keyboard.Space:
            action.set(4);
        case keyboard.R :
            action.set(5);
        case keyboard.E :
            action.set(6);
        // case keyboard. :
        //     action.set(7);
        // case keyboard. :
        //     action.set(8);
        case keyboard.T :
            action.set(9);
        case keyboard.D :
            action.set(10);
        case keyboard.L :
            action.set(11);
        case keyboard.Num1 :
            action.set(12);
        case keyboard.Num2 :
            action.set(13);
        case keyboard.Num3 :
            action.set(14);
        case keyboard.Num4 :
            action.set(15);
    }
    short short_action = (short)(action.to_ulong());
    
    return short_action;
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
                    sendPlayerInput(socket, playerId, action);
                    break;
                default:
                    break;
            }
        }

        window.clear(sf::Color::Black);
        gamerules->draw(window);
        window.display();
    } else {
        events.windowClosed = true;
    }
    return events;
}