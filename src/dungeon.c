#include "dungeon.h"

#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <string.h>
#include "cJSON.h"
#include "door.h"

char *loadFile(const char *fileName) {
    FILE *fp = fopen(fileName, "rb");
    if (fp == NULL) {
        perror("ERROR: fopen");
        return NULL;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return NULL;
    }

    long fileSize = ftell(fp);
    if (fileSize < 0) {
        fclose(fp);
        return NULL;
    }

    rewind(fp);

    char *buffer = malloc((size_t)fileSize + 1);
    if (buffer == NULL) {
        fclose(fp);
        return NULL;
    }

    size_t bytesRead = fread(buffer, 1, (size_t)fileSize, fp);

    if (bytesRead != (size_t)fileSize || ferror(fp)) {
        free(buffer);
        fclose(fp);
        return NULL;
    }

    buffer[bytesRead] = '\0';
    fclose(fp);

    return buffer;
}

int parseSpawnOffset(char *value, SpawnOffset *result) {
    if (strcmp(value, "UP") == 0) {
        *result = UP;
    } else if (strcmp(value, "RIGHT") == 0) {
        *result = RIGHT;
    } else if (strcmp(value, "DOWN") == 0) {
        *result = DOWN;
    } else if (strcmp(value, "LEFT") == 0) {
        *result = LEFT;
    } else {
        return -1;
    }

    return 0;
}

int parseKeyType(char *value, KeyType *result) {
    if (strcmp(value, "NONE") == 0) {
        *result = KEY_NONE;
    } else if (strcmp(value, "BRONZE") == 0) {
        *result = KEY_BRONZE;
    } else if (strcmp(value, "SILVER") == 0) {
        *result = KEY_SILVER;
    } else if (strcmp(value, "GOLD") == 0) {
        *result = KEY_GOLD;
    } else {
        return -1;
    }

    return 0;
}

void freeDungeon(Dungeon *dungeon) {
    if (dungeon == NULL) return;
    for (size_t i = 0; i < dungeon->roomCount; i++) {
        free(dungeon->rooms[i].doors);
        free(dungeon->rooms[i].chests);
    }

    free(dungeon->rooms);

    dungeon->rooms = NULL;
    dungeon->roomCount = 0;  
}

int loadDungeonConfig(Dungeon *dungeon) {
    TraceLog(LOG_INFO, "Loading Dungeon Configuration");
    dungeon->rooms = NULL;
    dungeon->roomCount = 0;

    char *buffer = loadFile("assets/dungeon.json");
    if (buffer == NULL) {
        return -1;
    }
    
    cJSON *dungeonJson = cJSON_Parse(buffer);
    free(buffer);
    if (dungeonJson == NULL) {
        return -1;
    }

    const cJSON *rooms;
    rooms = cJSON_GetObjectItemCaseSensitive(dungeonJson, "rooms");
    if (!cJSON_IsArray(rooms)) {
        goto error;
    }

    dungeon->roomCount = (size_t)cJSON_GetArraySize(rooms);
    if (dungeon->roomCount > 0) {
        dungeon->rooms = calloc(dungeon->roomCount, sizeof(Room));
        if (dungeon->rooms == NULL) goto error;
    }

    size_t i = 0;
    const cJSON *roomJson;
    cJSON_ArrayForEach(roomJson, rooms) {
        if (!cJSON_IsObject(roomJson)) goto error;

        const cJSON *id = cJSON_GetObjectItemCaseSensitive(roomJson, "id");
        if (!cJSON_IsNumber(id)) goto error;

        const cJSON *map = cJSON_GetObjectItemCaseSensitive(roomJson, "map");
        if (!cJSON_IsString(map)) goto error;

        const cJSON *doors = cJSON_GetObjectItemCaseSensitive(roomJson, "doors");
        if (!cJSON_IsArray(doors)) goto error;

        Room *room = &dungeon->rooms[i];
        room->id = (int)id->valuedouble;
        room->loaded = false;
        room->doorCount = (size_t)cJSON_GetArraySize(doors);
        TraceLog(LOG_DEBUG, "Doom Count: %i", room->doorCount);
        if (room->doorCount > 0) {
            room->doors = calloc(room->doorCount, sizeof(Door));
            if (room->doors == NULL) goto error;
        }

        snprintf(room->map, sizeof(room->map), "%s", map->valuestring);

        size_t j = 0;
        const cJSON *door;
        cJSON_ArrayForEach(door, doors) {
            if (!cJSON_IsObject(door)) goto error;

            const cJSON *id = cJSON_GetObjectItemCaseSensitive(door, "id");
            if (!cJSON_IsNumber(id)) goto error;

            const cJSON *x = cJSON_GetObjectItemCaseSensitive(door, "x");
            if (!cJSON_IsNumber(x)) goto error;

            const cJSON *y = cJSON_GetObjectItemCaseSensitive(door, "y");
            if (!cJSON_IsNumber(y)) goto error;

            const cJSON *destinationRoomId = cJSON_GetObjectItemCaseSensitive(door, "destinationRoomId");
            if (!cJSON_IsNumber(destinationRoomId)) goto error;

            const cJSON *destinationDoorId = cJSON_GetObjectItemCaseSensitive(door, "destinationDoorId");
            if (!cJSON_IsNumber(destinationDoorId)) goto error;

            const cJSON *destinationSpawnOffset = cJSON_GetObjectItemCaseSensitive(door, "destinationSpawnOffset");
            if (!cJSON_IsString(destinationSpawnOffset)) goto error;

            const cJSON *lockedJson = cJSON_GetObjectItemCaseSensitive(door, "locked");
            if (!cJSON_IsBool(lockedJson)) goto error;

            const cJSON *requiredKeyJson = cJSON_GetObjectItemCaseSensitive(door, "requiredKey");
            if (!cJSON_IsString(requiredKeyJson)) goto error;

            Door *door = &room->doors[j];
            door->id = (int)id->valuedouble;
            door->x = (int)x->valuedouble;
            door->y = (int)y->valuedouble;
            door->destinationRoomId = (int)destinationRoomId->valuedouble;
            door->destinationDoorId = (int)destinationDoorId->valuedouble;
            door->isLocked = cJSON_IsTrue(lockedJson);

            if (parseSpawnOffset(destinationSpawnOffset->valuestring, &door->spawnOffset) == -1) {
                TraceLog(LOG_ERROR, "Invalid spawn offset");
                goto error;
            }

            if (parseKeyType(requiredKeyJson->valuestring, &door->requiredKey) == -1) {
                TraceLog(LOG_ERROR, "Invalid key type");
                goto error;
            }

            j++;
            TraceLog(LOG_DEBUG, "Found Doom ID: %0.f", id->valuedouble);
        }

        const cJSON *chestsJson = cJSON_GetObjectItemCaseSensitive(roomJson, "chests");
        if (!cJSON_IsArray(chestsJson)) goto error;

        room->chestCount = (size_t)cJSON_GetArraySize(chestsJson);
        TraceLog(LOG_DEBUG, "Chest Count: %i", room->chestCount);

        if (room->chestCount > 0) {
            room->chests = calloc(room->chestCount, sizeof(Chest));
            if (room->chests == NULL) goto error;
        }

        size_t k = 0;
        const cJSON *chestJson;

        cJSON_ArrayForEach(chestJson, chestsJson) { 
            if (!cJSON_IsObject(chestJson)) goto error;

            const cJSON *idJson = cJSON_GetObjectItemCaseSensitive(chestJson, "id");
            if (!cJSON_IsNumber(idJson)) goto error;

            const cJSON *xJson = cJSON_GetObjectItemCaseSensitive(chestJson, "x");
            if (!cJSON_IsNumber(xJson)) goto error;

            const cJSON *yJson = cJSON_GetObjectItemCaseSensitive(chestJson, "y");
            if (!cJSON_IsNumber(yJson)) goto error;

            Chest *chest = &room->chests[k];
            chest->id = (int)idJson->valuedouble;
            chest->x = (int)xJson->valuedouble;
            chest->y = (int)yJson->valuedouble;
            chest->isOpen = false;

            k++;
            TraceLog(LOG_DEBUG, "Found Chest ID: %0.f", idJson->valuedouble);
        }

        i++;
        TraceLog(LOG_DEBUG, "Found Room ID: %0.f", id->valuedouble);
    }

    dungeon->currentRoomId = dungeon->rooms[0].id;
    cJSON_Delete(dungeonJson);
    TraceLog(LOG_INFO, "Dungeon configuration finish");
    return 0;

error:
    TraceLog(LOG_ERROR, "Error loading configuration");
    cJSON_Delete(dungeonJson);
    freeDungeon(dungeon);
    return -1;
}

Room *getRoomById(const Dungeon *dungeon, int id) { 
    for (int i = 0; i < dungeon->roomCount; i++) {
        if (dungeon->rooms[i].id != id) {
            continue;
        }

        return &dungeon->rooms[i];
    }

    return NULL;
}

Room *getCurrentRoom(const Dungeon *dungeon) {
    int currentRoomId = dungeon->currentRoomId;
    return getRoomById(dungeon, dungeon->currentRoomId);
}