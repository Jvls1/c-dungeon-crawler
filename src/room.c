#include "room.h"

const DoorConnection room1Doors[] = {
    {0, 10, FACING_UP}
};

int initializeRoom(Room *room, const Map *map) {
    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            int index = y * map->width + x;
            Tile tile = map->tiles[index];

            if (tile.type != DOOR) {
                continue;
            }
            TraceLog(LOG_DEBUG, "X: %i, Y: %i", x, y);
        }
    }
}

int handleRoomChange(Room *room, const Player *player, const Map *map) {
    // if (room.loaded) return 0;
    // char roomName[32];
    // snprintf(
    //     roomName,
    //     sizeof(roomName),
    //     "room_%02d.txt",
    //     room.number
    // );
    // if (readMap(roomName) == -1) {
    //     TraceLog(LOG_ERROR, "Unable to load new room");
    //     return -1;
    // };
    // if (initPlayerPosition() == -1) {
    //     TraceLog(LOG_ERROR, "Cannot initialize player position");
    //     return -1;
    // }
    // room.loaded = true;
    return 0;
}

int changeRoom(Room *room) {
    room->loaded = false;
}