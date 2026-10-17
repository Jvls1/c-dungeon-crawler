#include "room.h"

Door *findDoorByEvent(const Room *room, const GameEvents *gameEvents) {
    Door *door = NULL;
    for (int i = 0; i < room->doorCount; i++) {
        if (room->doors[i].x == gameEvents->tileX &&
            room->doors[i].y == gameEvents->tileY
        ) {
            door = &room->doors[i];
            TraceLog(LOG_INFO, "DestinationRoomId: %i", door->destinationRoomId);
        }
    }
    return door;
}

Door *findDoorById(const Room *room, const int doorId) { 
    Door *door = NULL;
    for (int i = 0; i < room->doorCount; i++) { 
        if (room->doors[i].id != doorId) continue;

        door = &room->doors[i];
        TraceLog(LOG_INFO, "DestinationDoorId: %i", door->id);
    }
    return door;
}

Chest *findChestByEvent(const Room *room, const GameEvents *gameEvents) {
    Chest *chest = NULL;
    for (int i = 0; i < room->chestCount; i++) {
        if (room->chests[i].x == gameEvents->tileX &&
            room->chests[i].y == gameEvents->tileY
        ) {
            chest = &room->chests[i];
            TraceLog(LOG_INFO, "Chest Id: %i", chest->id);
        }
    }
    return chest;
}