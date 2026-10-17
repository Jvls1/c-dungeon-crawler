#include "game.h"

#include <stdlib.h>

int handleRoomInteraction(Player *player, Map *map, Dungeon *dungeon, GameEvents *gameEvents);
int handleChestInteraction(Dungeon *dungeon, GameEvents *gameEvents);

int handleInterationRequest(Player *player, Map *map, Dungeon *dungeon, GameEvents *gameEvents) { 
    gameEvents->interactinoRequested = false;

    const int mapIndex = gameEvents->tileY * map->width + gameEvents->tileX;
    Tile interactionTile = map->tiles[mapIndex];

    if (!interactionTile.interactable) {
        TraceLog(LOG_WARNING, "Tile not interactable in handleInteractionRequest");
        return -1;
    }

    switch (interactionTile.type)
    {
    case DOOR:
        return handleRoomInteraction(player, map, dungeon, gameEvents);
    case CHEST: 
        return handleChestInteraction(dungeon, gameEvents);
        break;
    default:
        TraceLog(LOG_WARNING, "Interactable without interaction function");
        break;
    }


    return 0;
}

int handleRoomInteraction(Player *player, Map *map, Dungeon *dungeon, GameEvents *gameEvents) { 
    const Room *currentRoom = getCurrentRoom(dungeon);

    if (currentRoom == NULL) { 
        TraceLog(LOG_ERROR, "Current Room not found");
        return -1;
    }

    const Door *door = findDoorByEvent(currentRoom, gameEvents);

    if (door == NULL) {
        TraceLog(LOG_ERROR, "Destination Roor not found");
        return -1;
    }


    const Room *destinationRoom = getRoomById(dungeon, door->destinationRoomId);

    if (destinationRoom == NULL) {
        TraceLog(LOG_ERROR, "Fail to find destination room");
        return -1;
    }

    const Door *destinationDoor = findDoorById(destinationRoom, door->destinationDoorId);
    if (destinationDoor == NULL) {
        TraceLog(LOG_ERROR, "Destination Door not found");
        return -1;
    }

    Map nextMap = {0};

    if (readMap(&nextMap, destinationRoom->map) == -1) {
        TraceLog(LOG_ERROR, "Error reading map");
        return -1;
    }

    Player nextPlayer = *player;

    if (changePlayerPosition(
            &nextPlayer, 
            &nextMap, 
            destinationDoor->x, 
            destinationDoor->y, 
            door->spawnOffset
        ) == -1)
    {
        TraceLog(LOG_ERROR, "Cannot change player position");
        free(nextMap.tiles);
        return -1;
    }

    free(map->tiles);

    *map = nextMap;
    *player = nextPlayer;
    dungeon->currentRoomId = destinationRoom->id;   

    return 0;
}

int handleChestInteraction(Dungeon *dungeon, GameEvents *gameEvents) { 
    Room *currentRoom = getCurrentRoom(dungeon);

    if (currentRoom == NULL) { 
        TraceLog(LOG_ERROR, "Current Room not found");
        return -1;
    }
    
    Chest *chest = findChestByEvent(currentRoom, gameEvents);
    
    if (chest == NULL) { 
        TraceLog(LOG_ERROR, "Chest not found");
        return -1;
    }

    if (chest->isOpen) { 
        TraceLog(LOG_INFO, "Chest alredy opened");
        return 0;
    }

    chest->isOpen = true;

    return 0;
}