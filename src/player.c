#include "player.h"

#include <raylib.h>
#include <raymath.h>

int initPlayerPosition(Player *player, const Map *map, SpawnOffset spawnOffset) { 
    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            int index = y * map->width + x;
            Tile tile = map->tiles[index];
            if (tile.type != SPAWN) continue;

            int cX = x;
            int cY = y;

            switch (spawnOffset)
            {
            case UP: cY--; break;
            case DOWN: cY++; break;
            case RIGHT: cX++; break;
            case LEFT: cX--; break;
            default: return -1;
            }

            if (cY < 0 || cY >= map->height ||
                cX < 0 || cX >= map->width) {
                TraceLog(LOG_ERROR, "Invalid array access");
                return -1;
            }

            int mapIndex = cY * map->width + cX;
            if (!map->tiles[mapIndex].walkable) {
                TraceLog(LOG_ERROR, "Invalid start position, tile not walkable.");
                return -1;
            }

            player->position = (Vector2){
                cX * RENDER_TILE_SIZE + RENDER_TILE_SIZE / 2.0f,
                cY * RENDER_TILE_SIZE + RENDER_TILE_SIZE / 2.0f};
            return 0;
        }
    }

    return -1;
}

int changePlayerPosition(Player *player, const Map *map, int x, int y, SpawnOffset spawnOffset) { 
    TraceLog(LOG_INFO, "Original X: %i Y: %i", x, y);
    switch (spawnOffset)
    {
    case UP: y--; break;
    case DOWN: y++; break;
    case RIGHT: x++; break;
    case LEFT: x--; break;
    default: return -1;
    }

    float playerX = x * RENDER_TILE_SIZE + RENDER_TILE_SIZE / 2.0f;
    float playerY = y * RENDER_TILE_SIZE + RENDER_TILE_SIZE / 2.0f;
    TraceLog(LOG_INFO, "Trying to reach X: %.2f Y: %.2f", playerX, playerY);

    if (!isPlayerInsideScreen(playerX, playerY) ||
        isPlayerCollidingWithWall(map, playerX, playerY)) {
        TraceLog(LOG_ERROR, "Invalid position");
        return -1;
    }

    player->position.x = playerX;
    player->position.y = playerY;

    return 0;
}

void handleInteraction(const Player *player, const Map *map, Vector2 position, GameEvents *gameEvents) {
    int tileX = position.x / RENDER_TILE_SIZE;
    int tileY = position.y / RENDER_TILE_SIZE;
    Tile tile;
    int mapIndex = 0;
    switch (player->facing) {
    case FACING_UP: tileY--; break;
    case FACING_DOWN: tileY++; break;
    case FACING_RIGHT: tileX++; break;
    case FACING_LEFT: tileX--; break;
    default: return;
    } 

    mapIndex = tileY * map->width + tileX;
    tile = map->tiles[mapIndex];
    if (!tile.interactable) {
        return;
    }
    
    TraceLog(LOG_DEBUG, "Handling Interaction on Tile: X: %i Y: %i", tileX, tileY);

    switch (tile.type)
    {
    case DOOR:
        TraceLog(LOG_DEBUG, "Handling DOOR");
        gameEvents->roomChangedRequested = true;
        gameEvents->tileX = tileX;
        gameEvents->tileY = tileY;
        break;
    default:
        TraceLog(LOG_WARNING, "Interaction not handled");
        break;
    } 
}

void updatePlayerFacingPos(Player *player) {
    if (player->direction.x > 0) {
        player->facing = FACING_RIGHT;
    } else if (player->direction.x < 0) {
        player->facing = FACING_LEFT;
    } else if (player->direction.y > 0) {
        player->facing = FACING_DOWN;
    } else if (player->direction.y < 0) {
        player->facing = FACING_UP;
    }
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
    }
    return direction;
}

void updatePlayer(Player *player, const Map *map, GameEvents *gameEvents, const float dt) {
    bool interaction = false;
    Vector2 direction = handleInput(&interaction);
    player->direction = direction;
    Vector2 newPlayerPos = player->position;
    newPlayerPos.x += direction.x * dt * player->speed;
    updatePlayerFacingPos(player);
    if (isPlayerInsideScreen(newPlayerPos.x, newPlayerPos.y) &&
        !isPlayerCollidingWithWall(map, newPlayerPos.x, newPlayerPos.y)) {
        player->position.x = newPlayerPos.x;
    }

    if (interaction) { 
        handleInteraction(player, map, newPlayerPos, gameEvents);
        interaction = false;
    }

    newPlayerPos = player->position;
    newPlayerPos.y += direction.y * dt * player->speed;

    if (isPlayerInsideScreen(newPlayerPos.x, newPlayerPos.y) &&
        !isPlayerCollidingWithWall(map, newPlayerPos.x, newPlayerPos.y)) {
        player->position.y = newPlayerPos.y;
    }
}

void drawFacingHighlight(const Player *player) {
    Vector2 position = player->position;
    int tileX = position.x / RENDER_TILE_SIZE;
    int tileY = position.y / RENDER_TILE_SIZE;
    switch (player->facing) {
    case FACING_UP: tileY--; break;
    case FACING_DOWN: tileY++; break;
    case FACING_RIGHT: tileX++; break;
    case FACING_LEFT: tileX--; break;
    default: return;
    } 

    DrawRectangleLinesEx(
        (Rectangle){ tileX * RENDER_TILE_SIZE, tileY * RENDER_TILE_SIZE, RENDER_TILE_SIZE, RENDER_TILE_SIZE },
        2.f,
        Fade(GOLD, 0.5f)
    );
}

void drawPlayer(const Player *player) { 
    DrawCircleV(player->position, 16, RAYWHITE);
    drawFacingHighlight(player);
}
