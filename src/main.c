#include <time.h>
#include <raylib.h>

#include "physics.h"
#include "parallel.h"

float rand_float(float low, float high) {
    return ((float)rand() / (float)(RAND_MAX)) * fabs(low - high) + low;
}

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

void borders(Vector2 *pos, Vector2 *vel) {
    if (pos->x < R) {
        pos->x = R;
        vel->x *= -DAMPING;
    } else if (pos->x >= X - R) {
        pos->x = X - R;
        vel->x *= -DAMPING;
    }

    if (pos->y < R) {
        pos->y = R;
        vel->y *= -DAMPING;
    } else if (pos->y >= Y - R) {
        pos->y = Y - R;
        vel->y *= -DAMPING;
    }
}

Vector2 calc_next_vel(Vector2 pos, Vector2 prev_pos, float dt) {
    Vector2 pos_diff = diff(pos, prev_pos);
    Vector2 next_vel = scalar_mult(pos_diff, 1.0f / dt);
    
    if (isnan(next_vel.x) || isnan(next_vel.y)) {
        return (Vector2){0, 0};
    }
    
    return next_vel;
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
    
    OpenCLState cl_state = init_opencl(1);

    int frames = 0;
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        BeginDrawing();
        ClearBackground(BLACK);

        for (int i = 0; i < N; i++) {
            // gravity
            sys.velocities[i] = sum(scalar_mult(g, dt), sys.velocities[i]);
        }

        viscosity(&sys, dt);

        for (int i = 0; i < N; i++) {
            sys.prev_positions[i] = sys.positions[i];
            sys.positions[i] = sum(scalar_mult(sys.velocities[i], dt), sys.positions[i]);
        }

        adjust_spring(&sys, &sprs, dt);
        apply_spring(&sys, &sprs, dt);

        dd_relaxation(&sys, dt);

        for (int i = 0; i < N; i++) {
            sys.velocities[i] = calc_next_vel(sys.positions[i], sys.prev_positions[i], dt);
            borders(&sys.positions[i], &sys.velocities[i]);

            DrawText(TextFormat("%0.1f", 1.f / dt), 10, 10, 40, WHITE);
            DrawCircleV(sys.positions[i], R, color);
        }

        EndDrawing();

        if (++frames % FPS == 0) {
            shrink_springs(&sprs);
        }
    }

    CloseWindow();

    release_opencl(&cl_state);

    free_system(&sys);
    free_springs(&sprs);

    return 0;
}
