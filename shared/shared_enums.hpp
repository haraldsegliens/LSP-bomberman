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