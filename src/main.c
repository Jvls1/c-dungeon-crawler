#include <raylib.h>
#include <raymath.h>

#define SCREENWIDTH 800
#define SCREENHEIGHT 450

typedef struct {
    Vector2 position;
    float speed;
} Player;

Player player;

void drawCurrentPosition() {
    DrawText(TextFormat("[DEBUG] PlayerX: %.2f PlayerY: %.2f", player.position.x, player.position.y), 10, 30, 20, RAYWHITE);
}

void drawFps() {
    DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 20, RAYWHITE);
}

void draw() {
    BeginDrawing();

    ClearBackground(BLACK);

    drawFps();
    drawCurrentPosition();
    DrawCircleV(player.position, 50, RAYWHITE);

    EndDrawing();
}

bool isPlayerInsideScreen(Vector2 position) {
    const float radius = 50.0f;

    return position.x >= radius &&
           position.x <= SCREENWIDTH - radius &&
           position.y >= radius &&
           position.y <= SCREENHEIGHT - radius;
}

Vector2 calculateNewPlayerPosition(const float dt, Vector2 direction) {
    Vector2 newPlayerPos = player.position;
    newPlayerPos.x += direction.x * dt * player.speed;
    newPlayerPos.y += direction.y * dt * player.speed;
    return newPlayerPos;
}

Vector2 handleInput() {
    Vector2 direction = {0};
    if (IsKeyDown(KEY_D)) direction.x += 1;
    if (IsKeyDown(KEY_A)) direction.x -= 1;
    if (IsKeyDown(KEY_W)) direction.y -= 1;
    if (IsKeyDown(KEY_S)) direction.y += 1;

    if (Vector2Length(direction) > 0) {
        direction = Vector2Normalize(direction);
    }
    return direction;
}

void update() {
    const float dt = GetFrameTime();
    Vector2 direction = handleInput();
    Vector2 newPlayerPos = calculateNewPlayerPosition(dt, direction);
    if (isPlayerInsideScreen(newPlayerPos)) {
        player.position = newPlayerPos;
    }
}

int main(void){
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "C Dungeon Crawler");

    SetTargetFPS(60);

    player.position = (Vector2){ (float)SCREENWIDTH/2, (float)SCREENHEIGHT/2 };
    player.speed = 150.f;

    while (!WindowShouldClose()) {
        update();
        draw();
    }

    CloseWindow();

    return 0;
}