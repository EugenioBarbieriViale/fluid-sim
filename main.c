#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <raylib.h>


#define N 5000
#define RADIUS 5

// distance from borders when particles are initialized
#define SHIFT 15

// maximum velocity of a particle
#define VMAX 50

// range of possible integer velocities (from -number to number)
#define VEL_RANGE 5

#define square(x) ((x) * (x))

typedef struct {
    Vector2 pos;
    Vector2 vel;
} Particle;

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

float rand_float(float low, float high) {
    return ((float)rand() / (float)(RAND_MAX)) * abs(low - high) + low;
}

float rand_vel(void) {
    float x;
    for (;;) {
        x = (rand() % (2 * VEL_RANGE)) - VEL_RANGE;
        if (x != 0)
            return x;
    }
}

void init_gas(Particle gas[N], int X, int Y) {
    for (int n=0; n<N; n++) {
        gas[n].pos.x = rand_float(0,1) * (X - SHIFT * 2) + SHIFT;
        gas[n].pos.y = rand_float(0,1) * (Y - SHIFT * 2) + SHIFT;

        /* gas[n].vel = (Vector2){rand_float(-1,1) * VMAX, rand_float(-1,1) * VMAX}; */
        gas[n].vel = (Vector2){rand_vel() * VMAX, rand_vel() * VMAX};
    }
}

void borders(Particle *particle, int X, int Y) {
    if (particle->pos.x < RADIUS || particle->pos.x > X - RADIUS)
        particle->vel.x *= -1;

    if (particle->pos.y < RADIUS || particle->pos.y > Y - RADIUS)
        particle->vel.y *= -1;
}

/* void get_data(float vel) { */
    
/* } */

int main() {
    srand(time(NULL));

    const int X = 1000;
    const int Y =  800;
    const int FPS = 60;

    InitWindow(X, Y, "Gas Particles");
    SetTargetFPS(FPS);

    Particle gas[N];
    init_gas(gas, X, Y);

    float possible_vels[2*VEL_RANGE];

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        float vqm = 0;

        BeginDrawing();
        ClearBackground(BLACK);

        for (int i=0; i<N; i++) {
            borders(&gas[i], X, Y);
            gas[i].pos = VectorSum(gas[i].pos, VectorMult(gas[i].vel, dt));

            vqm += VectorSquare(gas[i].vel);

            printf("%f\n", sqrt(VectorSquare(gas[i].vel)));

            DrawCircleV(gas[i].pos, RADIUS, GRAY);
        }

        vqm = sqrt(vqm / (float)N);

        /* printf("%.1f\n", vqm); */

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
