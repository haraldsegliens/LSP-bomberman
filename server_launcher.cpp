#include <iostream>

#include "shared/gamerules.hpp"

int main() {
    std::cout << "Bomberman server launcher console application" << std::endl;
    int port;
    std::cout << "Enter server port: ";
    std::cin >> port;
    Gamerules gamerules(port);
    std::cin.get();
    return 0;
}