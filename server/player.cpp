#include "../shared/player.hpp"
#include <math.h>

Player::Player(int _id, 
               std::string _name, 
               sf::Vector2<float> _position,
               ConnectionWrapper* _con) : id(_id), name(_name),
                                               position(_position), 
                                               powerup(Powerup::NONE),
                                               power(1), speed(1),
                                               maxDynamiteCount(1),
                                               playerConnection(_con),
                                               playerInput(0),
                                               lastProcessedPlayerInput(0) {}

Player::~Player() {}

void Player::updateKeepAlive(Gamerules* gamerules) {
    if((gamerules->getCurrentTime() - lastKeepAlive).asSeconds() > TIMEOUT_DURATION) {
        destroy = true;
    }
}

void Player::update(Gamerules* gamerules) {
    updateKeepAlive();
    if(isTemporaryPowerup() && endTemporaryPowerup < gamerules->getCurrentTime()) {
        powerup = Powerup::NONE;
    }
    SurroundingInfo info = gamerules->scanSurrounding(gamerules,position);
    handleSurroundings(gamerules,info);
    handlePlayerInput(info);
}

void Player::handleSurroundings(Gamerules* gamerules, SurroundingInfo& info) {
    if(info.containsWorldCell(WorldCell::WALL) || 
       info.containsWorldCell(WorldCell::BOX) ||
       info.findVolatileEntity(VolatileEntityType::FIRE) != nullptr) {
        dead = true;
        return;
    }
    VolatileEntity* powerupEntity = info.findVolatileEntity(VolatileEntityType::FIRE);
    if(powerupEntity != null) {
        powerup = powerupEntity->powerupType;
        endTemporaryPowerup = gamerules->getCurrentTime() + sf::seconds(POWERUP_DURATION);
        gamerules->getVolatileEntitiesManager()->deleteEntity(powerupEntity);
        info.entities.remove(powerupEntity);
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

    sf::Vector2<int> freeGround = info.findWorldCell(WorldCell::GROUND);
    if(isOneTimePressed(PlayerInputBits::PLANT_BOMB)) {
        if(currentDynamites.size() < maxDynamiteCount && 
            info.entities.size() == 0 && info.dynamites.size() == 0 && 
            freeGround.x != -1) {
            //create dynamite
            info.dynamites.push_back(gamerules->createDynamite(freeGround,this));
        }
    }

    if(isOneTimePressed(PlayerInputBits::DETONATE_REMOTELY)) {
        if(powerup == Powerup::KICK_DYNAMITE) {
            for(auto& it : currentDynamites) {
                *currentDynamites.explode();
            }
            currentDynamites.clear();
        }
    }
}

void Player::movePlayer(Gamerules* gamerules) {
    Vector2<float> cell_position(floor(position.x) + 0.5f, floor(position.y) + 0.5f);
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
    Vector2<float> targetCell = cell_position + direction;
    SurroundingInfo targetInfo = gamerules->scanSurrounding(gamerules, targetCell);
    if(!targetInfo.containsWorldCell(WorldCell::GROUND)) {
        return;
    }

    if(targetInfo.players.size() == 2 || 
        (targetInfo.players.size() == 1 && targetInfo.players[0] != this)) {
        return;
    }

    //handling dynamite kick powerup
    if(powerup == Powerup::DYNAMITE_KICK && targetInfo.dynamites.size() == 1) {
        targetInfo.dynamites[0]->kick(direction);
    } else {
        return;
    }
    if(direction.x != 0) {
        //horizontāli
        position.x += getUnitSpeed() * gamerules->getDeltaTime().asSeconds() * direction.x;
        position.y = cell_position.y;
    } else {
        //vertikāli
        position.x = cell_position.x;
        position.y += getUnitSpeed() * gamerules->getDeltaTime().asSeconds() * direction.y;
    }
}

void Player::removeDynamite(Dynamite* dynamite) {
    currentDynamites.remove(currentDynamites);
}