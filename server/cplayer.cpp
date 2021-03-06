#include "../shared/player.hpp"
#include <math.h>
#include <iostream>

Player::Player(int _id, 
               std::string _name, 
               sf::Vector2<float> _position,
               Connection* _con,
               Gamerules* gamerules) : id(_id), name(_name), 
                                               direction(sf::Vector2<int>(1,0)), 
                                               powerup(Powerup::NONE),power(1), 
                                               speed(1),maxDynamiteCount(1),
                                               dead(false),playerConnection(_con),
                                               lastKeepAlive(gamerules->getCurrentTime()),
                                               ready(false),destroy(false),
                                               playerInput(0), lastProcessedPlayerInput(0) {
    position = sf::Vector2f(floor(_position.x) + 0.5f,floor(_position.y) + 0.5f);
}

Player::~Player() {}

void Player::updateKeepAlive(Gamerules* gamerules) {
    if((gamerules->getCurrentTime() - lastKeepAlive).asSeconds() > TIMEOUT_DURATION) {
        std::cout << "Player not responding with KEEPALIVE(name = " << name << ")" << std::endl;
        destroy = true;
    }
}

void Player::update(Gamerules* gamerules) {
    updateKeepAlive(gamerules);
    if(isTemporaryPowerup() && endTemporaryPowerup < gamerules->getCurrentTime()) {
        powerup = Powerup::NONE;
    }
    SurroundingInfo info = gamerules->scanSurrounding(position,PLAYER_CHECKBOX_SIZE_RELATIVE);
    handleSurroundings(gamerules,info);
    handlePlayerInput(gamerules,info);
}

void Player::handleSurroundings(Gamerules* gamerules, SurroundingInfo& info) {
    if(info.containsWorldCell(WorldCell::WALL) || 
       info.containsWorldCell(WorldCell::BOX) ||
       info.findVolatileEntity(VolatileEntityType::FIRE) != nullptr) {
        dead = true;
        std::cout << "DEAD" << std::endl;
        return;
    }
    VolatileEntity* powerupEntity = info.findVolatileEntity(VolatileEntityType::POWERUP);
    if(powerupEntity != nullptr) {
        switch(powerupEntity->powerupType) {
            case Powerup::DYNAMITE_POWER:
                power++;
                break;
            case Powerup::RUNNING_SPEED:
                speed++;
                break;
            case Powerup::DYNAMITE_COUNT:
                maxDynamiteCount++;
                break;
            case Powerup::REMOTE_DETONATOR:
            case Powerup::DYNAMITE_KICK:
                powerup = powerupEntity->powerupType;
                endTemporaryPowerup = gamerules->getCurrentTime() + sf::seconds(POWERUP_DURATION);
                break;
            default:
                break;
        }
        gamerules->getVolatileEntitiesManager()->deleteEntity(powerupEntity);
        info.entities.erase(std::find(
            info.entities.begin(), info.entities.end(), powerupEntity
        ));
        return;
    }
}

void Player::handlePlayerInput(Gamerules* gamerules, SurroundingInfo& info) {
    if(isPressed(PlayerInputBits::UP)) {
        direction.x = 0;
        direction.y = -1;
        movePlayer(gamerules);
    } else if(isPressed(PlayerInputBits::DOWN)) {
        direction.x = 0;
        direction.y = 1;
        movePlayer(gamerules);
    } else if(isPressed(PlayerInputBits::LEFT)) {
        direction.x = -1;
        direction.y = 0;
        movePlayer(gamerules);
    } else if(isPressed(PlayerInputBits::RIGHT)) {
        direction.x = 1;
        direction.y = 0;
        movePlayer(gamerules);
    }

    if(isOneTimePressed(PlayerInputBits::PLANT_BOMB) && 
       currentDynamites.size() < maxDynamiteCount && info.entities.size() == 0 &&
       info.dynamites.size() == 0) {
        Dynamite* dynamite = gamerules->createDynamite(
            sf::Vector2f(floor(position.x)+0.5f,floor(position.y)+0.5f),
            power,this
        );
        currentDynamites.push_back(dynamite);
        info.dynamites.push_back(dynamite);
    }
    
    if(isOneTimePressed(PlayerInputBits::DETONATE_REMOTELY)) {
        if(powerup == Powerup::REMOTE_DETONATOR) {
            for(Dynamite* dynamite : currentDynamites) {
                dynamite->explode(gamerules);
            }
            currentDynamites.clear();
        }
    }
}

void Player::movePlayer(Gamerules* gamerules) {
    sf::Vector2f cell_position(floor(position.x) + 0.5f, floor(position.y) + 0.5f);
    if(direction.x != 0) {
        //horizontāli
        if(cell_position.y - PLAYER_INCELL_RANGE/2 > position.y &&
           cell_position.y + PLAYER_INCELL_RANGE/2 < position.y ) {
            return;
        }
    } else {
        //vertikāli
        if(cell_position.x - PLAYER_INCELL_RANGE/2 > position.x &&
           cell_position.x + PLAYER_INCELL_RANGE/2 < position.x ) {
            return;
        }
    }
    sf::Vector2f targetPosition = position;
    if(direction.x != 0) {
        //horizontāli
        targetPosition.x += getUnitSpeed() * gamerules->getDeltaTime().asSeconds() * direction.x;
        targetPosition.y = cell_position.y;
    } else {
        //vertikāli
        targetPosition.x = cell_position.x;
        targetPosition.y += getUnitSpeed() * gamerules->getDeltaTime().asSeconds() * direction.y;
    }
    SurroundingInfo currentInfo = gamerules->scanSurrounding(position,PLAYER_CHECKBOX_SIZE_RELATIVE);
    SurroundingInfo targetInfo = gamerules->scanSurrounding(targetPosition,PLAYER_CHECKBOX_SIZE_RELATIVE);
    if(targetInfo.containsWorldCell(WorldCell::WALL) || targetInfo.containsWorldCell(WorldCell::BOX)) {
        return;
    }

    if(targetInfo.players.size() == 2 || 
        (targetInfo.players.size() == 1 && targetInfo.players[0] != this)) {
        return;
    }

    //handling dynamite kick powerup
    if(targetInfo.dynamites.size() == 1) {
        if(powerup == Powerup::DYNAMITE_KICK && currentInfo.dynamites.size() == 0) {
            targetInfo.dynamites[0]->kick(direction);
        } else if(currentInfo.dynamites.size() == 0) {
            return;
        }
    }
    position = targetPosition;
}

void Player::removeDynamite(Dynamite* dynamite) {
    auto it = std::find(currentDynamites.begin(),currentDynamites.end(),dynamite);
    if(it != currentDynamites.end()) {
        currentDynamites.erase(it);
    }
}

sf::Vector2f Player::getPosition() {return position;}
void Player::setPosition(sf::Vector2f a) {
    position = a;
}