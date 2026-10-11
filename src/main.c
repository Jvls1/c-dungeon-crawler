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

typedef enum {
    FACING_UP,
    FACING_DOWN,
    FACING_RIGHT,
    FACING_LEFT
} Facing;

typedef struct {
    Vector2 position;
    Vector2 direction;
    Facing facing;
    float speed;
} Player;

typedef struct {
    int number;
    bool loaded;
    Facing entryFacing;
} Room;

typedef enum {
    FLOOR,
    WALL,
    DOOR,
    SPAWN
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

Player player;
Map map;
Room room;

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

int initPlayerPosition() {
    for (int x = 0; x < map.width; x++) {
        for (int y = 0; y < map.height; y++) {
            int index = y * map.width + x;
            int cX = x;
            int cY = y;
            Tile tile = map.tiles[index];
            if (tile.type != SPAWN) continue;

            switch (room.entryFacing)
            {
            case FACING_UP: cY--; break;
            case FACING_DOWN: cY++; break;
            case FACING_RIGHT: cX++; break;
            case FACING_LEFT: cX--; break;
            default: return -1;
            }

            if (cY < 0 || cY >= map.height ||
                cX < 0 || cX >= map.width) {
                TraceLog(LOG_ERROR, "Invalid array access");
                return -1;
            }
            int mapIndex = cY * map.width + cX;
            if (!map.tiles[mapIndex].walkable) {
                TraceLog(LOG_ERROR, "Invalid start position, tile not walkable.");
                return -1;
            }

            player.position = (Vector2){
                cX * RENDER_TILE_SIZE + RENDER_TILE_SIZE / 2.0f,
                cY * RENDER_TILE_SIZE + RENDER_TILE_SIZE / 2.0f};
            return 0;
        }
    }
    return -1;
}

int readMap(const char *fileName) {
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
            TraceLog(LOG_ERROR, "Too many tiles in map.");
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
        TraceLog(LOG_ERROR, "number of tiles different");
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
    DrawText(TextFormat("[DEBUG] Direction DirX: %.2f DirY: %.2f", player.direction.x, player.direction.y), 10, 70, 20, RAYWHITE);
    DrawText(TextFormat("[DEBUG] Facing: %i", (int)player.facing), 10, 90, 20, RAYWHITE);
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

void drawFacingHighlight() {
    Vector2 position = player.position;
    int tileX = position.x / RENDER_TILE_SIZE;
    int tileY = position.y / RENDER_TILE_SIZE;
    switch (player.facing) {
    case FACING_UP:
        tileY--;
        break;
    case FACING_DOWN:
        tileY++;
        break;
    case FACING_RIGHT:
        tileX++;
        break;
    case FACING_LEFT:
        tileX--;
        break;
    default:
        return;
    } 

    DrawRectangleLinesEx(
        (Rectangle){ tileX * RENDER_TILE_SIZE, tileY * RENDER_TILE_SIZE, RENDER_TILE_SIZE, RENDER_TILE_SIZE },
        2.f,
        Fade(GOLD, 0.5f)
    );
}

void drawMap() {
    for (int x = 0; x < map.width; x++) {
        for (int y = 0; y < map.height; y++) {
            int index = y * map.width + x;
            Tile tile = map.tiles[index];
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

void drawFps() {
    DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 20, RAYWHITE);
}

void draw() {
    BeginDrawing();

    ClearBackground(BLACK);

    drawMap();
    drawFps();
    drawCurrentPosition();
    drawFacingHighlight();
    DrawCircleV(player.position, 16, RAYWHITE);

    EndDrawing();
}

int handleRoomChange() {
    if (room.loaded) return 0;
    char roomName[32];
    snprintf(
        roomName,
        sizeof(roomName),
        "room_%02d.txt",
        room.number
    );
    if (readMap(roomName) == -1) {
        TraceLog(LOG_ERROR, "Unable to load new room");
        return -1;
    };
    if (initPlayerPosition() == -1) {
        TraceLog(LOG_ERROR, "Cannot initialize player position");
        return -1;
    }
    room.loaded = true;
    return 0;
}

void changeRoom() {
    room.number++;
    room.loaded = false;
    room.entryFacing = player.facing;
}

void handleInteraction(Tile *tile) {
    switch (tile->type)
    {
    case DOOR:
        TraceLog(LOG_DEBUG, "Handling DOOR");
        changeRoom();
        break;
    default:
        TraceLog(LOG_WARNING, "Interaction not handled");
        break;
    } 
}

void consumeInteraction(Vector2 position) {
    Vector2 direction = player.direction;
    int tileX = position.x / RENDER_TILE_SIZE;
    int tileY = position.y / RENDER_TILE_SIZE;
    Tile tile;
    int mapIndex = 0;
    switch (player.facing) {
    case FACING_UP:
        tileY--;
        break;
    case FACING_DOWN:
        tileY++;
        break;
    case FACING_RIGHT:
        tileX++;
        break;
    case FACING_LEFT:
        tileX--;
        break;
    default:
        return;
    } 

    mapIndex = tileY * map.width + tileX;
    tile = map.tiles[mapIndex];

    if (tile.interactable) {
        TraceLog(LOG_DEBUG, "Interaction");
        handleInteraction(&tile);
    }
}

void updatePlayerFacingPos() {
    if (player.direction.x > 0) {
        player.facing = FACING_RIGHT;
    } else if (player.direction.x < 0) {
        player.facing = FACING_LEFT;
    } else if (player.direction.y > 0) {
        player.facing = FACING_DOWN;
    } else if (player.direction.y < 0) {
        player.facing = FACING_UP;
    }
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

Vector2 handleInput(bool *interaction) {
    Vector2 direction = {0};
    if (IsKeyDown(KEY_D)) direction.x += 1;
    if (IsKeyDown(KEY_A)) direction.x -= 1;
    if (IsKeyDown(KEY_W)) direction.y -= 1;
    if (IsKeyDown(KEY_S)) direction.y += 1;
    if (IsKeyPressed(KEY_E)) *interaction = true;

    if (Vector2Length(direction) > 0) {
        direction = Vector2Normalize(direction);
        player.direction = direction;
    }
    return direction;
}

void update() {
    const float dt = GetFrameTime();
    bool interaction = false;
    Vector2 direction = handleInput(&interaction);
    Vector2 newPlayerPos = player.position;
    newPlayerPos.x += direction.x * dt * player.speed;
    updatePlayerFacingPos();
    if (isPlayerInsideScreen(newPlayerPos) &&
        !isPlayerCollidingWithWall(newPlayerPos)) {
        player.position.x = newPlayerPos.x;
    }

    if (interaction) { 
        consumeInteraction(newPlayerPos);
        interaction = false;
    }

    newPlayerPos = player.position;
    newPlayerPos.y += direction.y * dt * player.speed;

    if (isPlayerInsideScreen(newPlayerPos) &&
        !isPlayerCollidingWithWall(newPlayerPos)) {
        player.position.y = newPlayerPos.y;
    }
    handleRoomChange();
}

int main(void){
    SetTraceLogLevel(LOG_ALL);
    SetTraceLogCallback(CDungeonCrawlerLog);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "C Dungeon Crawler");

    SetTargetFPS(60);
    
    if (readMap("room_01.txt") == -1) {
        CloseWindow();
        return 1;
    }

    room.number = 1;
    room.loaded = true;
    room.entryFacing = FACING_UP;
    if (initPlayerPosition() == -1) {
        TraceLog(LOG_ERROR, "Cannot initialize player position");
        CloseWindow();
        return 1;
    }

    // player.position = (Vector2){ (float)SCREEN_WIDTH/2, (float)SCREEN_HEIGHT/2 };
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