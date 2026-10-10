#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 500
#define TILE_SIZE 16
#define MAP_SCALE 3
#define RENDER_TILE_SIZE (TILE_SIZE * MAP_SCALE)

typedef struct {
    Vector2 position;
    float speed;
} Player;

typedef enum {
    FLOOR,
    WALL,
    DOOR,
    SPAWN
} TileType;

typedef struct {
    TileType type;
    bool walkable;
} Tile;

typedef struct {
    int width;
    int height;
    Tile *tiles;
} Map;

Player player;
Map map;

int charToTile(char ch, Tile *tile) {
    switch (ch) {
    case '#':
        *tile = (Tile) { .type = WALL, .walkable = false };
        break;
    case '.':
        *tile = (Tile) { .type = FLOOR, .walkable = true };
        break;
    case 'D':
        *tile = (Tile) { .type = DOOR, .walkable = false };
        break;
    case 'P':
        *tile = (Tile) { .type = SPAWN, .walkable = true };
        break;
    default:
        printf("ERROR: undefined Tile '%c'", ch);
        printf("\n");
        return -1;
    }
    return 1;
}

int readMap() {
    FILE *fp = fopen("./assets/maps/room_01.txt", "r");    
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
                printf("ERROR: different line sizes.");
                printf("\n");
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
        printf("ERROR: different line sizes.");
        printf("\n");
        fclose(fp);
        return -1;
    } else if (width != 0) {
        height++;
    }

    printf("DEBUG: width: %i", expectedWidth);
    printf("\n");
    printf("DEBUG: height: %i", height);
    printf("\n");
    size_t totalTiles = (size_t) expectedWidth * (size_t) height;
    map.tiles = malloc(totalTiles * sizeof *map.tiles);
    if (map.tiles == NULL) {
        perror("ERROR: malloc");
        fclose(fp);
        return -1;
    }

    map.width = expectedWidth;
    map.height = height;
    rewind(fp);
    int i = 0;
    while ((ch = fgetc(fp)) != EOF) {        
        if (ch == '\n' || ch == '\r') {
            continue;
        }

        if ((size_t)i >= totalTiles) {
            printf("ERROR: too many tiles in map.\n");
            free(map.tiles);
            map.tiles = NULL;
            fclose(fp);
            return -1;
        }

        Tile tile;
        if (charToTile(ch, &tile) == -1) {
            free(map.tiles);
            map.tiles = NULL;
            fclose(fp);
            return -1;
        }
        map.tiles[i] = tile;
        i++;
    }
    if (i != (expectedWidth * height)) {
        printf("ERROR: number of tiles different");
        printf("\n");
        free(map.tiles);
        map.tiles = NULL;
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

void drawCurrentPosition() {
    DrawText(TextFormat("[DEBUG] RawPos PlayerX: %.2f PlayerY: %.2f", player.position.x, player.position.y), 10, 30, 20, RAYWHITE);
    int xTilePosition = player.position.x / RENDER_TILE_SIZE;
    int yTilePosition = player.position.y / RENDER_TILE_SIZE;
    DrawText(TextFormat("[DEBUG] MapPos PlayerX: %i PlayerY: %i", xTilePosition, yTilePosition), 10, 50, 20, RAYWHITE);
}

void drawMap() {
    for (int x = 0; x < map.width; x++) {
        for (int y = 0; y < map.height; y++) {
            int index = y * map.width + x;
            Tile tile = map.tiles[index];
            Color color;
            switch (tile.type)
            {
            case WALL:
                color = GRAY;
                break;
            case FLOOR:
                color = DARKGREEN;
                break;
            default:
                color = PINK;
                break;
            }

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

void drawFps() {
    DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 20, RAYWHITE);
}

void draw() {
    BeginDrawing();

    ClearBackground(BLACK);

    drawMap();
    drawFps();
    drawCurrentPosition();
    DrawCircleV(player.position, 16, RAYWHITE);

    EndDrawing();
}

bool isPlayerCollidingWithWall(Vector2 position) {
    for (int y = 0; y < map.height; y++) {
        for (int x = 0; x < map.width; x++) {
            int index = y * map.width + x;
            Tile tile = map.tiles[index];

            if (tile.walkable) {
                continue;
            }

            Rectangle wall = {
                x * RENDER_TILE_SIZE,
                y * RENDER_TILE_SIZE,
                RENDER_TILE_SIZE,
                RENDER_TILE_SIZE};

            if (CheckCollisionCircleRec(position, 16.f, wall)) {
                return true;
            }
        }
    }

    return false;
}
 
bool isPlayerInsideScreen(Vector2 position) {
    const float radius = 16.f;

    return position.x >= radius &&
           position.x <= SCREEN_WIDTH - radius &&
           position.y >= radius &&
           position.y <= SCREEN_HEIGHT - radius;
}

Vector2 calculateNewPlayerPosition(const float dt, Vector2 direction) {
    Vector2 newPlayerPos = player.position;
    newPlayerPos.x += direction.x * dt * player.speed;
    newPlayerPos.y += direction.y * dt * player.speed;
    return newPlayerPos;
}

Vector2 handleInput() {
    Vector2 direction = {0};
    if (IsKeyDown(KEY_D)) direction.x += 1;
    if (IsKeyDown(KEY_A)) direction.x -= 1;
    if (IsKeyDown(KEY_W)) direction.y -= 1;
    if (IsKeyDown(KEY_S)) direction.y += 1;

    if (Vector2Length(direction) > 0) {
        direction = Vector2Normalize(direction);
    }
    return direction;
}

void update() {
    const float dt = GetFrameTime();
    Vector2 direction = handleInput();

    Vector2 newPlayerPos = player.position;
    newPlayerPos.x += direction.x * dt * player.speed;

    if (isPlayerInsideScreen(newPlayerPos) &&
        !isPlayerCollidingWithWall(newPlayerPos)) {
        player.position.x = newPlayerPos.x;
    }

    newPlayerPos = player.position;
    newPlayerPos.y += direction.y * dt * player.speed;

    if (isPlayerInsideScreen(newPlayerPos) &&
        !isPlayerCollidingWithWall(newPlayerPos)) {
        player.position.y = newPlayerPos.y;
    }
}

int main(void){
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "C Dungeon Crawler");

    SetTargetFPS(60);
    
    if (readMap() == -1) {
        CloseWindow();
        return 1;
    }

    player.position = (Vector2){ (float)SCREEN_WIDTH/2, (float)SCREEN_HEIGHT/2 };
    player.speed = 150.f;

    while (!WindowShouldClose()) {
        update();
        draw();
    }

    free(map.tiles);
    map.tiles = NULL;

    CloseWindow();

    return 0;
}