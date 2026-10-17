#ifndef GAME_EVENTS_H
#define GAME_EVENTS_H

#include <stdbool.h>

typedef struct {
    bool interactinoRequested;
    int tileX;
    int tileY;
} GameEvents;

#endif