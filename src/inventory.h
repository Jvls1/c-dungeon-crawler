#ifndef INVENTORY_H
#define INVENTORY_H

#include "item_type.h"

typedef struct {
    unsigned int itemCounts[ITEM_TYPE_COUNT];
} Inventory;

#endif