#ifndef ROOM_H
#define ROOM_H

#include <raylib.h>
#include <stddef.h>
#include "spawn_offset.h"
#include "game_events.h"
#include "door.h"
#include "chest.h"

#define MAP_NAME_SIZE 32

typedef struct {
    int id;
    char map[MAP_NAME_SIZE];
    bool loaded;

    Door *doors;
    size_t doorCount;
    
    Chest *chests;
    size_t chestCount;
} Room;

Door *findDoorByEvent(const Room *room, const GameEvents *gameEvents);
Door *findDoorById(const Room *room, const int doorId);
Chest *findChestByEvent(const Room *room, const GameEvents *gameEvents);

#endif