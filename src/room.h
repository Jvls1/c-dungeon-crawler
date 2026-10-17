#ifndef ROOM_H
#define ROOM_H

#include <raylib.h>
#include <stddef.h>
#include "spawn_offset.h"
#include "game_events.h"
#include "door.h"

#define ROOM_H 
#define MAP_NAME_SIZE 32

typedef struct {
    int id;
    char map[MAP_NAME_SIZE];
    bool loaded;
    Door *doors;
    size_t doorCount;
} Room;

const Door *findDoorByEvent(const Room *room, const GameEvents *gameEvents);
const Door *findDoorById(const Room *room, const int doorId);

#endif