#include <iostream>
#include <string>
#include <stdlib.h>

#include "shared/gamerules.hpp"

int main(int argc, char* argv[]) {
    std::cout << "Bomberman client launcher console application" << std::endl;
    Gamerules gamerules(std::string(argv[1]),atoi(argv[2]),std::string(argv[3]));

    while(gamerules.getState() == GameState::NOT_CONNECTED) {}

    if(gamerules.getState() == GameState::CONNECTION_ERROR) {
        std::cout << "Connection error" << std::endl;
        return 1;
    }
    gamerules.ready();
    while(gamerules.getState() != GameState::END && gamerules.getState() != GameState::CONNECTION_ERROR) {}

    if(gamerules.getState() == GameState::CONNECTION_ERROR) {
        std::cout << "Connection error" << std::endl;
        return 1;
    }
    
    return 0;
}