#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>

#include "player.h"
#include "map.h"
#include "room.h"
#include "dungeon.h"
#include "game.h"

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
    DrawText(
        TextFormat(
            "[DEBUG] Bronze Key: %i Silver Key: %i, Gold Key: %i",
            player->inventory.itemCounts[1],
            player->inventory.itemCounts[2],
            player->inventory.itemCounts[3]),
        10,
        110,
        20,
        RAYWHITE);
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

void update(Player *player, Map *map, Dungeon *dungeon, GameEvents *gameEvents) {
    const float dt = GetFrameTime();
    updatePlayer(player, map, gameEvents, dt);

    if (gameEvents->interactinoRequested) {
        if (handleInterationRequest(player, map, dungeon, gameEvents) == -1) {
            TraceLog(LOG_ERROR, "Error handling interaction");
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

    Inventory inventory = {
        .itemCounts[ITEM_BRONZE_KEY] = 1,
        .itemCounts[ITEM_SILVER_KEY] = 1,
        .itemCounts[ITEM_GOLD_KEY] = 1,
    };
    Player player = {
        .speed = 250.f,
        .inventory = inventory
    };

    GameEvents gameEvents = {0};
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