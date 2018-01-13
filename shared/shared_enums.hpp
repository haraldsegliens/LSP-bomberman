typedef enum { 
    NOT_CONNECTED = 0,
    LOBBY = 1,
    INIT = 2,
    GAME = 3
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