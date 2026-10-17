#ifndef MAP_H
#define MAP_H

#include <raylib.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 500
#define TILE_SIZE 16
#define MAP_SCALE 3
#define RENDER_TILE_SIZE (TILE_SIZE * MAP_SCALE)

typedef enum {
    FLOOR,
    WALL,
    DOOR,
    SPAWN,
    CHEST
} TileType;

typedef struct {
    TileType type;
    bool walkable;
    bool interactable;
} Tile;

typedef struct {
    int width;
    int height;
    Tile *tiles;
} Map;

int readMap(Map *map, const char *fileName);
bool isPlayerCollidingWithWall(const Map *map, int x, int y); 
bool isPlayerInsideScreen(int x, int y);
void drawMap(const Map *map);

#endif