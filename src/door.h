#ifndef DOOR_H
#define DOOR_H

#include "spawn_offset.h"

typedef enum {
    KEY_NONE,
    KEY_BRONZE,
    KEY_SILVER,
    KEY_GOLD
} KeyType;

typedef struct {
    int id;
    int x;
    int y;

    int destinationRoomId;
    int destinationDoorId;
    SpawnOffset spawnOffset;
    
    bool isLocked;
    KeyType requiredKey;
} Door;

#endif