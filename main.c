#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>


#define N 10
#define R 8.31
#define radius 10

typedef struct {
    Vector2 pos;
    Vector2 vel;
    Vector2 acc;
} Particle;

float rand_float() {
    return (float)rand() / (float)RAND_MAX;
}

Vector2 VectorSum(Vector2 v, Vector2 w) {
    return (Vector2){v.x + w.x, v.y + w.y};
}

Vector2 VectorMult(Vector2 v, float scalar) {
    return (Vector2){v.x * scalar, v.y * scalar};
}

void init_gas(Particle gas[N], int X, int Y, int vmax) {
    for (int n=0; n<N; n++) {
        gas[n].pos = (Vector2){rand_float() * X, rand_float() * Y};
        gas[n].vel = (Vector2){rand_float() * vmax, rand_float() * vmax};
        gas[n].acc = (Vector2){rand_float(), rand_float()};
    }
}


int main() {
    srand(time(NULL));

    const int X = 1000;
    const int Y =  800;
    const int FPS = 30;

    InitWindow(X, Y, "Gas Particles");
    SetTargetFPS(FPS);

    const int vmax = 1e2;

    Particle gas[N];
    init_gas(gas, X, Y, vmax);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        BeginDrawing();
        ClearBackground(BLACK);

        for (int i=0; i<N; i++) {
            gas[i].pos = VectorSum(gas[i].pos, VectorMult(gas[i].vel, dt));
            DrawCircleV(gas[i].pos, radius, GRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
