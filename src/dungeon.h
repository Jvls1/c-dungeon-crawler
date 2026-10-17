#ifndef DUNGEON_H
#define DUNGEON_H

#include "room.h"

typedef struct {
    int currentRoomId;
    Room *rooms;
    size_t roomCount;
} Dungeon;

int loadDungeonConfig(Dungeon *dungeon);
void freeDungeon(Dungeon *dungeon);
const Room *getCurrentRoom(const Dungeon *dungeon);
const Room *getRoomById(const Dungeon *dungeon, int id);

#endif