#include <time.h>
#include <raylib.h>

#include "physics.h"
#include "parallel.h"

void alloc_system(ParticleSystem *sys) {
    sys->positions = (Vector2*)malloc(N * sizeof(Vector2));
    sys->prev_positions = (Vector2*)malloc(N * sizeof(Vector2));
    sys->velocities = (Vector2*)malloc(N * sizeof(Vector2));
}

void free_system(ParticleSystem *sys) {
    free(sys->positions);
    free(sys->prev_positions);
    free(sys->velocities);
}

void init_system(ParticleSystem *sys) {
    if (sys->positions && sys->prev_positions && sys->velocities) {
        for (int i = 0; i < N; i++) {
            sys->positions[i] = (Vector2){rand_float(R, X / 2.f - R), rand_float(R, Y - R)};
            sys->prev_positions[i] = sys->positions[i];
            sys->velocities[i] = (Vector2){0, 0};
        }
    }
}


int main() {
    srand(time(NULL));

    InitWindow(X, Y, "Fluid Simulation");
    SetTargetFPS(FPS);

    ParticleSystem sys;

    alloc_system(&sys);
    init_system(&sys);

    Springs sprs;
    alloc_springs(&sprs, 128);
    
    // OpenCLState cl_state = init_opencl(1);

    int frames = 0;
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        BeginDrawing();
        ClearBackground(BLACK);

        update(&sys, &sprs, &frames, dt);

        EndDrawing();
    }

    CloseWindow();

    // release_opencl(&cl_state);

    free_system(&sys);
    free_springs(&sprs);

    return 0;
}
