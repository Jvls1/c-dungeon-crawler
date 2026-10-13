#!/bin/bash

set -e

mkdir -p build

clang \
    -std=c17 \
    -Wall \
    -Wextra \
    src/main.c \
    src/player.c \
    src/map.c \
    src/room.c \
    -o build/game \
    $(pkg-config --cflags --libs raylib)

echo "build successful"
echo "run with: ./build/game"