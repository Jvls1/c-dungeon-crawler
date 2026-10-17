#!/bin/bash

set -e

mkdir -p build

clang \
    -std=c17 \
    -Wall \
    -Wextra \
    -Ilibs \
    src/main.c \
    src/player.c \
    src/map.c \
    src/room.c \
    src/door.c \
    src/dungeon.c \
    src/game.c \
    libs/cJSON.c \
    -o build/game \
    $(pkg-config --cflags --libs raylib)

echo "build successful"
echo "run with: ./build/game"