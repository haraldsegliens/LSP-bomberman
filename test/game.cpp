#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <bitset>

using namespace std;

int connectToServer(const char* IP, int PORT)
{
    // copied from 
    // https://www.geeksforgeeks.org/socket-programming-cc/
    ///////////////////////////////////////////////////////
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        // socket connection problem
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, IP, &serv_addr.sin_addr)<=0) 
    {
        // invalid address
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        // connection failed
        return -1;
    }

    return sock;
}

void sendJoinRequest(int socket, string playerName)
{
    // cropping player input name to allowed size
    if(playerName.length() < 23)
    {
        playerName += '\0';
    }
    string request = '1'+playerName.substr(0,23);
    send(socket, request.data(), request.length(), 0);
}

void sendKeepAlive(int socket, char playerId)
{
    string request = "3";
    request += playerId;
    send(socket, request.data(), 2, 0);
}

void sendReady(int socket, char playerId)
{
    string request = "4";
    request += playerId;
    send(socket, request.data(), 2, 0);
}

// checking from sfml what keys are pressed, creates short integer to send in packet
short bindActionToShort(sf::Event::KeyEvent keyPressed)
{
    bitset<16> action;
    action.reset();
    sf::Keyboard keyboard;
    switch (keyPressed.code)
    {
        case keyboard.Up :
            action.set(0);
        case keyboard.Down :
            action.set(1);
        case keyboard.Left :
            action.set(2);
        case keyboard.Right :
            action.set(3);
        case keyboard.Space :
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

void sendPlayerInput(int socket, char playerId, short action)
{
    string request = "4";
    request += playerId;
    request += (action & 0xff);
    request += ((action >> 8) & 0xff);
    send(socket, request.data(), 4, 0);
}

int main()
{
    const char* IP = "127.0.0.1";
    const int PORT = 8080;

    int socket = connectToServer(IP, PORT);

    sf::RenderWindow window(sf::VideoMode(800, 800), "BOMBERMAN");

    char playerId = 'X';

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                // window closed
                case sf::Event::Closed:
                {
                    window.close();

                    break;
                }
                case sf::Event::KeyPressed:
                {
                    // user is giving some input
                    short action = bindActionToShort(event.key);
                    sendPlayerInput(socket, playerId, action);
                }
                default:
                    break;
            }
        }
        window.clear(sf::Color::Black);

        window.display();
    }

    return 0;
}