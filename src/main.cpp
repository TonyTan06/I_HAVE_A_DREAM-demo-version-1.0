#include "scene.h"
#include "raylib.h"

int main() {
    InitWindow(800, 450, "I HAVE A DREAM");
    SetTargetFPS(60);

    Scene scene;

    while (!WindowShouldClose()) {
        scene.update(GetFrameTime());

        BeginDrawing();
        scene.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
