#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "map.h"
#include "dungeon.h"

int handleInterationRequest(Player *player, Map *map, Dungeon *dungeon, GameEvents *gameEvents);
bool isVictoryConditionMet(const Dungeon *dungeon);

#endif