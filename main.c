// TODO:
//  - continous collision detection

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <raylib.h>


#define N 5000
#define RADIUS 5

#define VMAX 1.0e2

#define square(x) ((x) * (x))

typedef struct {
    Vector2 pos;
    Vector2 vel;
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

float VectorSquare(Vector2 v) {
    return (float)(VectorMagn(v) * VectorMagn(v));
}

float rand_poss(int n) {
    return (rand() % n) - n*2;
}

void init_gas(Particle gas[N], int X, int Y) {
    int shift = 30 + RADIUS;
    
    for (int n=0; n<N; n++) {
        gas[n].pos.x = rand_float(0,1) * (X - shift * 2) + shift;
        gas[n].pos.y = rand_float(0,1) * (Y - shift * 2) + shift;

        gas[n].vel = (Vector2){rand_float(-1,1) * VMAX, rand_float(-1,1) * VMAX};
        /* gas[n].vel = (Vector2){rand_poss(5) * VMAX, rand_poss(5) * VMAX}; */
    }
}

void borders(Particle *particle, int X, int Y) {
    if (particle->pos.x < RADIUS || particle->pos.x > X - RADIUS)
        particle->vel.x *= -1;

    if (particle->pos.y < RADIUS || particle->pos.y > Y - RADIUS)
        particle->vel.y *= -1;
}

int main() {
    srand(time(NULL));

    const int X = 1000;
    const int Y =  800;
    const int FPS = 60;

    InitWindow(X, Y, "Gas Particles");
    SetTargetFPS(FPS);

    Particle gas[N];
    init_gas(gas, X, Y);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        float vqm = 0;

        BeginDrawing();
        ClearBackground(BLACK);

        for (int i=0; i<N; i++) {
            borders(&gas[i], X, Y);
            gas[i].pos = VectorSum(gas[i].pos, VectorMult(gas[i].vel, dt));

            vqm += VectorSquare(gas[i].vel);

            DrawCircleV(gas[i].pos, RADIUS, GRAY);
        }

        vqm = sqrt(vqm / (float)N);

        printf("%f\n", vqm);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
