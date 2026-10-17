#include "game.h"

#include <stdlib.h>

int handleDoorInteraction(Player *player, Map *map, Dungeon *dungeon, GameEvents *gameEvents);
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
        return handleDoorInteraction(player, map, dungeon, gameEvents);
    case CHEST: 
        return handleChestInteraction(dungeon, gameEvents);
        break;
    default:
        TraceLog(LOG_WARNING, "Interactable without interaction function");
        break;
    }


    return 0;
}

int handleDoorInteraction(Player *player, Map *map, Dungeon *dungeon, GameEvents *gameEvents) { 
    Room *currentRoom = getCurrentRoom(dungeon);

    if (currentRoom == NULL) { 
        TraceLog(LOG_ERROR, "Current Room not found");
        return -1;
    }

    Door *interactionDoor = findDoorByEvent(currentRoom, gameEvents);

    if (interactionDoor == NULL) {
        TraceLog(LOG_ERROR, "Destination Door not found");
        return -1;
    }

    if (interactionDoor->isLocked) {
        TraceLog(LOG_DEBUG, "Door locked, need key of type %i", interactionDoor->requiredKey);

        bool hasKey;

        switch (interactionDoor->requiredKey)
        {
        case KEY_BRONZE:
            hasKey = player->hasBronzeKey;
            break; 
        case KEY_SILVER:
            hasKey = player->hasSilverKey;
            break;
        case KEY_GOLD: 
            hasKey = player->hasGoldKey;
            break;
        default:
            TraceLog(LOG_ERROR, "Invalid Key Type");
            return -1;
        }

        if (!hasKey) {
            TraceLog(LOG_INFO, "Need Key to continue");
            return 0;
        }

        TraceLog(LOG_DEBUG, "Door opened");
    }

    Room *destinationRoom = getRoomById(dungeon, interactionDoor->destinationRoomId);

    if (destinationRoom == NULL) {
        TraceLog(LOG_ERROR, "Fail to find destination room");
        return -1;
    }

    Door *destinationDoor = findDoorById(destinationRoom, interactionDoor->destinationDoorId);
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
            interactionDoor->spawnOffset
        ) == -1)
    {
        TraceLog(LOG_ERROR, "Cannot change player position");
        free(nextMap.tiles);
        return -1;
    }

    free(map->tiles);

    destinationDoor->isLocked = false;
    interactionDoor->isLocked = false;

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