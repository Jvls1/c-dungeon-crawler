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
Room *getCurrentRoom(const Dungeon *dungeon);
Room *getRoomById(const Dungeon *dungeon, int id);

#endif