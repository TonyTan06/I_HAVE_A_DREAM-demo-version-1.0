#include "player.h"
#include "enemy.h"
#include "item.h"
#include "character.h"
#include "raylib.h"

#include <iostream>
#include <string>

int main() {
    Player player ("PLAYER");

    
    InitWindow(800, 450, "I HAVE A DREAM");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Hello raylib!", 280, 200, 30, BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}