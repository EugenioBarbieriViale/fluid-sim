#include <stdio.h>
#include <raylib.h>

int main() {
    const int X = 1000;
    const int Y =  800;
    const int FPS = 30;

    InitWindow(X, Y, "Gas Particles");
    SetTargetFPS(FPS);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
