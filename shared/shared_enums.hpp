#ifndef SHARED_ENUMS_H
#define SHARED_ENUMS_H

#define MINIMUM_PLAYERS 1

#define FIRE_DURATION 3.0f
#define TIMEOUT_DURATION 10.0f
#define POWERUP_DURATION 15.0f
#define POWERUP_ALIVE_DURATION 60.0f
#define DYNAMITE_DURATION 5.0f
#define PLAYER_INCELL_RANGE 0.8f
#define PLAYER_BASE_SPEED 2.0f
#define PLAYER_INCREASE_SPEED 2.0f
#define PLAYER_CHECKBOX_SIZE_RELATIVE 0.8f

#define DYNAMITE_KICK_SPEED 10.0f
#define DYNAMITE_KICK_DEACCELERATION 4.0f

#define SPRITE_CELL_SIZE 16
#define SPRITE_PLAYER_SIZE_WIDTH 16
#define SPRITE_PLAYER_SIZE_HEIGTH 24
#define SPRITE_DYNAMITE_SIZE 14

typedef enum {
    NOT_CONNECTED = 0,
    LOBBY = 1,
    INIT = 2,
    GAME = 3,
    END = 4,
    CONNECTION_ERROR = 5
} GameState;

typedef enum {
    NONE = -1,
    DYNAMITE_POWER = 0,
    RUNNING_SPEED = 1,
    REMOTE_DETONATOR = 2,
    DYNAMITE_COUNT = 3,
    DYNAMITE_KICK = 4
} Powerup;

typedef enum {
    GROUND = 0,
    WALL = 1,
    BOX = 2
} WorldCell;

typedef enum {
    JOIN_REQUEST = 1,
    JOIN_RESPONSE = 2,
    KEEP_ALIVE = 3,
    READY = 4,
    PLAYER_INPUT = 5,
    DISCONNECT = 6,
    LOBBY_STATUS = 7,
    GAME_START = 8,
    MAP_UPDATE = 9,
    OBJECTS = 10,
    GAME_OVER = 11
} PacketType;

typedef enum {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3,
    PLANT_BOMB = 4,
    DETONATE_REMOTELY = 5,
    KICK_DYNAMITE = 6
} PlayerInputBits;

#endif