#include <iostream>

#include "shared/gamerules.hpp"

int main() {
    std::cout << "Bomberman server launcher console application" << std::endl;
    int port;
    std::cout << "Enter server port: ";
    std::cin >> port;
    Gamerules gamerules(port);
    int i;
    std::cin >> i;
    std::cout << i << std::endl;
    return 0;
}