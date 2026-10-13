#include "map.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int charToTile(char ch, Tile *tile) {
    switch (ch) {
    case '#':
        *tile = (Tile) { .type = WALL, .walkable = false, .interactable = false };
        break;
    case '.':
        *tile = (Tile) { .type = FLOOR, .walkable = true, .interactable = false };
        break;
    case 'D':
        *tile = (Tile) { .type = DOOR, .walkable = false, .interactable = true };
        break;
    case 'P':
        *tile = (Tile) { .type = SPAWN, .walkable = true, .interactable = false };
        break;
    default:
        TraceLog(LOG_ERROR, "Undefined Tile '%c'", ch);
        return -1;
    }
    return 1;
}

int readMap(Map *map, const char *fileName) {
    char basePath[256] = "./assets/maps/";
    strcat(basePath, fileName);
    FILE *fp = fopen(basePath, "r");    
    if (fp == NULL) {
        perror("ERROR: fopen");
        return -1;
    }

    int expectedWidth = 0;
    int width = 0;
    int height = 0;
    int ch;
    bool firstLine = true;
    while ((ch = fgetc(fp)) != EOF) {        
        if (ch == '\r') {
            continue;
        }

        if (ch == '\n') {
            height++;
            if (!firstLine && width != expectedWidth) {
                TraceLog(LOG_ERROR, "Different line sizes");
                fclose(fp);
                return -1;
            }
            firstLine = false;
            width = 0;
            continue;
        }
        if (firstLine) {
            expectedWidth++;
        }
        width++;
    }

    if (width != 0 && width != expectedWidth) {
        TraceLog(LOG_ERROR, "Different line sizes");
        fclose(fp);
        return -1;
    } else if (width != 0) {
        height++;
    }

    TraceLog(LOG_DEBUG, "Map width '%i'", expectedWidth);
    TraceLog(LOG_DEBUG, "Map height '%i'", height);
    size_t totalTiles = (size_t) expectedWidth * (size_t) height;
    map->tiles = malloc(totalTiles * sizeof *map->tiles);
    if (map->tiles == NULL) {
        perror("ERROR: malloc");
        fclose(fp);
        return -1;
    }

    map->width = expectedWidth;
    map->height = height;
    rewind(fp);
    int i = 0;
    while ((ch = fgetc(fp)) != EOF) {        
        if (ch == '\n' || ch == '\r') {
            continue;
        }

        if ((size_t)i >= totalTiles) {
            TraceLog(LOG_ERROR, "Too many tiles in map.");
            free(map->tiles);
            map->tiles = NULL;
            fclose(fp);
            return -1;
        }

        Tile tile;
        if (charToTile(ch, &tile) == -1) {
            free(map->tiles);
            map->tiles = NULL;
            fclose(fp);
            return -1;
        }
        map->tiles[i] = tile;
        i++;
    }
    if (i != (expectedWidth * height)) {
        TraceLog(LOG_ERROR, "number of tiles different");
        free(map->tiles);
        map->tiles = NULL;
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

bool isPlayerCollidingWithWall(const Map *map, int pX, int pY) {
    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            int index = y * map->width + x;
            Tile tile = map->tiles[index];

            if (tile.walkable) {
                continue;
            }

            Rectangle wall = {
                x * RENDER_TILE_SIZE,
                y * RENDER_TILE_SIZE,
                RENDER_TILE_SIZE,
                RENDER_TILE_SIZE};

            // TODO: remove the 16.f or pass by parameter
            if (CheckCollisionCircleRec((Vector2){ pX, pY }, 16.f, wall)) {
                return true;
            }
        }
    }

    return false;
}

bool isPlayerInsideScreen(int x, int y) {
    // TODO: remove this or pass by parameter
    const float radius = 16.f;

    return x >= radius &&
           x <= SCREEN_WIDTH - radius &&
           y >= radius &&
           y <= SCREEN_HEIGHT - radius;
    
}

Color getTileColor(const Tile *tile) {
    switch (tile->type)
    {
    case WALL:
        return GRAY;
    case FLOOR:
        return DARKGREEN;
    default:
        return PINK;
    }
}

void drawMap(const Map *map) {
    for (int x = 0; x < map->width; x++) {
        for (int y = 0; y < map->height; y++) {
            int index = y * map->width + x;
            Tile tile = map->tiles[index];
            Color color = getTileColor(&tile);

            DrawRectangle(
                x * RENDER_TILE_SIZE,
                y * RENDER_TILE_SIZE,
                RENDER_TILE_SIZE,
                RENDER_TILE_SIZE,
                color
            );
        }
    }
}