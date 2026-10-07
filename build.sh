#!/bin/bash

set -e

mkdir -p build

clang \
    -std=c17 \
    -Wall \
    -Wextra \
    src/main.c \
    -o build/game \
    $(pkg-config --cflags --libs raylib)

echo "build successful"
echo "run with: ./build/game"