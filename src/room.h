#ifndef ROOM_H
#define ROOM_H

#include <raylib.h>
#include "player.h"

typedef struct {
    int x;
    int y;
    Facing entryFacing;
} DoorConnection;

typedef struct {
    bool loaded;
    DoorConnection *doors;
    Facing entryFacing;
} Room;

int handleRoomChange(Room *room, const Player *player, const Map *map);
int changeRoom(Room *room);

#endif