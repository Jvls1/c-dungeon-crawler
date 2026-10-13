#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include "map.h"

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

int initPlayerPosition(Player *player, const Map *map);
void updatePlayer(Player *player, const Map *map, const float dt);
void drawPlayer(const Player *player);

#endif