// TODO:
//  - continous collision detection

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <raylib.h>


#define N 1000
#define RADIUS 4

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

void init_gas(Particle gas[N], int X, int Y) {
    int shift = 30;
    
    for (int n=0; n<N; n++) {
        gas[n].pos.x = rand_float(0,1) * (X - RADIUS - shift) + shift;
        gas[n].pos.y = rand_float(0,1) * (Y - RADIUS - shift) + shift;

        gas[n].vel = (Vector2){rand_float(-1,1) * VMAX, rand_float(-1,1) * VMAX};
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

        Vector2 v_tot = {0,0};
        float vqm_v = 0;

        BeginDrawing();
        ClearBackground(BLACK);

        for (int i=0; i<N; i++) {
            borders(&gas[i], X, Y);
            gas[i].pos = VectorSum(gas[i].pos, VectorMult(gas[i].vel, dt));

            vqm += square(VectorMagn(gas[i].vel));

            v_tot = VectorSum(v_tot, gas[i].vel);
            vqm_v = VectorSquare(v_tot);

            DrawCircleV(gas[i].pos, RADIUS, GRAY);
        }

        vqm = sqrt(vqm / (float)N);
        vqm_v = sqrt(vqm_v / (float)N);

        /* printf("S: %f\n", vqm); */
        printf("V: %f\n", vqm_v);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
