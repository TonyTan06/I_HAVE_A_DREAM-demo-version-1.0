#include "scene.h"
#include "raylib.h"

int main() {
    // 创建 raylib 游戏窗口；场景自身负责输入、更新和绘制。
    InitWindow(800, 450, "I HAVE A DREAM");
    SetTargetFPS(60);

    Scene scene;

    while (!WindowShouldClose()) {
        // GetFrameTime() 是上一帧实际经过的秒数，保证不同帧率下移动速度一致。
        scene.update(GetFrameTime());

        // raylib 的每帧绘制必须位于 BeginDrawing/EndDrawing 之间。
        BeginDrawing();
        scene.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
