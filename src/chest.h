#ifndef CHEST_H
#define CHEST_H

#include "item_type.h"

typedef struct {
    int id;
    int x;
    int y;
    bool isOpen;
    ItemType itemType;
    unsigned int quantity;
} Chest;

#endif