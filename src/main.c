// TODO:
// 1. pass actual dt to kernel (not target)

#include <time.h>
#include <raylib.h>

// #include "springs.h" // remember to free springs
#include "parallel.h"

void alloc_system(ParticleSystem *sys, int N) {
    sys->positions = (vector2*)malloc(N * sizeof(vector2));
    sys->prev_positions = (vector2*)malloc(N * sizeof(vector2));
    sys->velocities = (vector2*)malloc(N * sizeof(vector2));
}

void free_system(ParticleSystem *sys) {
    free(sys->positions);
    free(sys->prev_positions);
    free(sys->velocities);
}

void init_system(ParticleSystem *sys, int N, float R, float X, float Y) {
    if (sys->positions && sys->prev_positions && sys->velocities) {
        for (int i = 0; i < N; i++) {
            sys->positions[i] = (vector2){rand_float(R, X / 2.f - R), rand_float(R, Y - R)};
            sys->prev_positions[i] = sys->positions[i];
            sys->velocities[i] = (vector2){0, 0};
        }
    }
}


int main() {
    srand(time(NULL));

    int FPS = 60;

    Constants consts = init_consts(FPS);

    InitWindow(consts.X, consts.Y, "Fluid Simulation");
    SetTargetFPS(FPS);

    ParticleSystem sys;

    alloc_system(&sys, consts.N);
    init_system(&sys, consts.N, consts.R, consts.X, consts.Y);

    Springs sprs;
    // alloc_springs(&sprs, 128);
    
    OpenCLState cl_state = init_opencl(consts, 0);

    // int frames = 0;
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        BeginDrawing();
        ClearBackground(BLACK);

        // update(&sys, &sprs, &frames, dt);
        parallel_compute(&cl_state, &sys, &sprs);

        for (int i = 0; i < consts.N; i++) {
            DrawText(TextFormat("%0.1f", 1.f / dt), 10, 10, 40, WHITE);
            DrawCircle(sys.positions[i].x, sys.positions[i].y, consts.R, color);
        }

        EndDrawing();
    }

    CloseWindow();

    release_opencl(&cl_state);

    free_system(&sys);
    // free_springs(&sprs);

    return 0;
}
