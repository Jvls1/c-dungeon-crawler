#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include "map.h"
#include "game_events.h"
#include "spawn_offset.h"

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
    
    //TODO: move this to inventory when the time come
    bool hasBronzeKey;
    bool hasSilverKey;
    bool hasGoldKey;
} Player;

int initPlayerPosition(Player *player, const Map *map, SpawnOffset spawnOffset);
int changePlayerPosition(Player *player, const Map *map, int x, int y, SpawnOffset spawnOffset);
void updatePlayer(Player *player, const Map *map, GameEvents *gameEvents, float dt);
void drawPlayer(const Player *player);

#endif