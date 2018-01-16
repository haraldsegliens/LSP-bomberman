#include <iostream>
#include <string>

#include "shared/gamerules.hpp"

int main() {
    std::cout << "Bomberman client launcher console application" << std::endl;
    std::string addr;
    int port;
    std::string name;
    std::cout << "Enter remote server address: ";
    std::cin >> addr;
    std::cout << "Enter remote server port: ";
    std::cin >> port;
    std::cout << "Enter name: ";
    std::cin >> name;
    
    Gamerules gamerules(addr,port,name);

    while(gamerules.getState() == GameState::NOT_CONNECTED) {}

    if(gamerules.getState() == GameState::CONNECTION_ERROR ) {
        std::cout << "Connection error" << std::endl;
        std::cin.get();
        return 1;
    }
    std::cin.get();
    gamerules.ready();
    std::cin.get();
    if(gamerules.getState() == GameState::LOBBY || 
       gamerules.getState() == GameState::INIT || 
       gamerules.getState() == GameState::GAME) {
        gamerules.disconnectClient();
    }
    
    return 0;
}