#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>

#include "player.h"
#include "map.h"
#include "room.h"
#include "dungeon.h"

void CDungeonCrawlerLog(int msgType, const char *text, va_list args) {
    switch (msgType)
    {
        case LOG_INFO: printf("[INFO]: "); break;
        case LOG_ERROR: printf("[ERROR]: "); break;
        case LOG_WARNING: printf("[WARN]: "); break;
        case LOG_DEBUG: printf("[DEBUG]: "); break;
        default: break;
    }

    vprintf(text, args);
    printf("\n");
}

void drawCurrentPosition(const Player *player) {
    DrawText(TextFormat("[DEBUG] RawPos PlayerX: %.2f PlayerY: %.2f", player->position.x, player->position.y), 10, 30, 20, RAYWHITE);
    int xTilePosition = player->position.x / RENDER_TILE_SIZE;
    int yTilePosition = player->position.y / RENDER_TILE_SIZE;
    DrawText(TextFormat("[DEBUG] MapPos PlayerX: %i PlayerY: %i", xTilePosition, yTilePosition), 10, 50, 20, RAYWHITE);
    DrawText(TextFormat("[DEBUG] Direction DirX: %.2f DirY: %.2f", player->direction.x, player->direction.y), 10, 70, 20, RAYWHITE);
    DrawText(TextFormat("[DEBUG] Facing: %i", (int)player->facing), 10, 90, 20, RAYWHITE);
}

void drawFps() {
    DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 20, RAYWHITE);
}

void draw(const Player *player, const Map *map) {
    BeginDrawing();

    ClearBackground(BLACK);

    drawMap(map);
    drawPlayer(player);
    drawFps();
    drawCurrentPosition(player);

    EndDrawing();
}

// TODO: move this to other file
int handleRoomTransition(Player *player, Map *map, Dungeon *dungeon, GameEvents *gameEvents) {
    gameEvents->roomChangedRequested = false;
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

void update(Player *player, Map *map, Dungeon *dungeon, GameEvents *gameEvents) {
    const float dt = GetFrameTime();
    updatePlayer(player, map, gameEvents, dt);

    if (gameEvents->roomChangedRequested) {
        if (handleRoomTransition(player, map, dungeon, gameEvents) == -1) {
            TraceLog(LOG_ERROR, "Error handling room transition");
        }
    }
}

int main(void){
    SetTraceLogLevel(LOG_ALL);
    SetTraceLogCallback(CDungeonCrawlerLog);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "C Dungeon Crawler");

    SetTargetFPS(60);

    Dungeon dungeon;
    if (loadDungeonConfig(&dungeon) == -1) { 
        CloseWindow();
        return 1;
    }

    Map map;
    if (readMap(&map, dungeon.rooms[0].map) == -1) {
        CloseWindow();
        return 1;
    }

    Player player = {
        .speed = 250.f
    };

    GameEvents gameEvents;
    if (initPlayerPosition(&player, &map, UP) == -1) {
        TraceLog(LOG_ERROR, "Cannot initialize player position");
        CloseWindow();
        return 1;
    }

    while (!WindowShouldClose()) {
        update(&player, &map, &dungeon, &gameEvents);
        draw(&player, &map);
    }

    freeDungeon(&dungeon);
    free(map.tiles);
    map.tiles = NULL;

    CloseWindow();

    return 0;
}