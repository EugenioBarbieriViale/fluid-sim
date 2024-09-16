#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <raylib.h>


#define N 10
#define radius 10

typedef struct {
    Vector2 pos;
    Vector2 vel;
    Vector2 acc;
} Particle;

float rand_float(float low, float high) {
    return ((float)rand() / (float)(RAND_MAX)) * abs(low - high) + low;
}

Vector2 VectorSum(Vector2 v, Vector2 w) {
    return (Vector2){v.x + w.x, v.y + w.y};
}

Vector2 VectorDiff(Vector2 v, Vector2 w) {
    return (Vector2){v.x - w.x, v.y - w.y};
}

Vector2 VectorMult(Vector2 v, float scalar) {
    return (Vector2){v.x * scalar, v.y * scalar};
}

float VectorMagn(Vector2 v) {
    return (float)sqrt(v.x*v.x + v.y*v.y);
}

void init_gas(Particle gas[N], int X, int Y, int vmax) {
    for (int n=0; n<N; n++) {
        gas[n].pos = (Vector2){rand_float(0,1) * (X-radius) + radius, rand_float(0,1) * (Y-radius) + radius};
        gas[n].vel = (Vector2){rand_float(-1,1) * vmax, rand_float(-1,1) * vmax};
        gas[n].acc = (Vector2){rand_float(-1,1), rand_float(-1,1)};
    }
}

void borders(Particle *particle, int X, int Y) {
    if (particle->pos.x < radius || particle->pos.x > X - radius)
        particle->vel.x *= -1;

    if (particle->pos.y < radius || particle->pos.y > Y - radius)
        particle->vel.y *= -1;
}

void collision(Particle *particle, Particle gas[N]) {
    for (int i=0; i<N; i++) {
        float dist = VectorMagn(VectorDiff(particle->pos, gas[i].pos));
        if (dist <= 2*radius) {
            particle->vel.x *= -1;
            break;
        }
    }
}

int main() {
    srand(time(NULL));

    const int X = 1000;
    const int Y =  800;
    const int FPS = 30;

    InitWindow(X, Y, "Gas Particles");
    SetTargetFPS(FPS);

    const int vmax = 2e2;

    Particle gas[N];
    init_gas(gas, X, Y, vmax);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        BeginDrawing();
        ClearBackground(BLACK);

        for (int i=0; i<N; i++) {
            gas[i].pos = VectorSum(gas[i].pos, VectorMult(gas[i].vel, dt));

            borders(&gas[i], X, Y);
            collision(&gas[i], gas);

            DrawCircleV(gas[i].pos, radius, GRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
