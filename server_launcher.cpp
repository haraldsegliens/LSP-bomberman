#include <iostream>
#include <csignal>

#include "shared/gamerules.hpp"

volatile sig_atomic_t stop;

void inthand(int signum) {
    stop = 1;
}

int main() {
	signal(SIGINT, inthand);

    std::cout << "Bomberman server launcher console application" << std::endl;
    int port;
    std::cout << "Enter server port: ";
    std::cin >> port;
    Gamerules gamerules(port);
    while(!stop) {}
    return 0;
}