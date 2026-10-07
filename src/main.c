#include <raylib.h>

typedef struct {
    Vector2 position;
} Player;

Player player;

void draw() {
    BeginDrawing();

    ClearBackground(BLACK);

    DrawCircleV(player.position, 50, RAYWHITE);

    EndDrawing();
}

void handleInput() {
    if (IsKeyDown(KEY_D)) player.position.x += 2.0f;
    if (IsKeyDown(KEY_A)) player.position.x -= 2.0f;
    if (IsKeyDown(KEY_W)) player.position.y -= 2.0f;
    if (IsKeyDown(KEY_S)) player.position.y += 2.0f;
}

void update() {
    handleInput();
}


int main(void){
    int screenWidth = 800;
    int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "C Dungeon Crawler");

    SetTargetFPS(60);

    player.position = (Vector2){ (float)screenWidth/2, (float)screenHeight/2 };

    while (!WindowShouldClose()) {
        update();
        draw();
    }

    CloseWindow();

    return 0;
}