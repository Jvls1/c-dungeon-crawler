#ifndef DOOR_H
#define DOOR_H

#include "spawn_offset.h"

typedef struct {
    int id;
    int x;
    int y;
    int destinationRoomId;
    int destinationDoorId;
    SpawnOffset spawnOffset;
} Door;

#endif