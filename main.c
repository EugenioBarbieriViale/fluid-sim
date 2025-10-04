#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <raylib.h>

#include "vect.h"
#include "consts.h"


float rand_float(float low, float high) {
    return ((float)rand() / (float)(RAND_MAX)) * fabs(low - high) + low;
}

typedef struct {
    Vector2 *positions;
    Vector2 *prev_positions;
    Vector2 *velocities;
} ParticleSystem;

typedef struct {
    int *is;
    int *js;
    float *rest_lengths;
    int count;
    int capacity;
} Springs;

void alloc_system(ParticleSystem *sys) {
    sys->positions = (Vector2*)malloc(N * sizeof(Vector2));
    sys->prev_positions = (Vector2*)malloc(N * sizeof(Vector2));
    sys->velocities = (Vector2*)malloc(N * sizeof(Vector2));
}

void init_system(ParticleSystem *sys) {
    if (sys->positions && sys->prev_positions && sys->velocities) {
        for (int i=0; i<N; i++) {
            sys->positions[i] = (Vector2){rand_float(R, X / 2.f - R), rand_float(R, Y - R)};
            sys->prev_positions[i] = sys->positions[i];
            sys->velocities[i] = (Vector2){0, 0};
        }
    }
}

void free_system(ParticleSystem *sys) {
    free(sys->positions);
    free(sys->prev_positions);
    free(sys->velocities);
}

void alloc_springs(Springs *sprs, int capacity) {
    sprs->is = (int*)malloc(capacity * sizeof(int));
    sprs->js = (int*)malloc(capacity * sizeof(int));
    sprs->rest_lengths = (float*)malloc(capacity * sizeof(float));
    sprs->count = 0;
    sprs->capacity = capacity;
}

void realloc_springs(Springs *sprs, int capacity) {
    sprs->is = (int*)realloc(sprs->is, capacity * sizeof(int));
    sprs->js = (int*)realloc(sprs->js, capacity * sizeof(int));
    sprs->rest_lengths = (float*)realloc(sprs->rest_lengths, capacity * sizeof(float));
    sprs->capacity = capacity;
}

int find_spring(const Springs *sprs, int i, int j) {
    for (int idx = 0; idx < sprs->count; idx++) {
        if ((sprs->is[idx] == i && sprs->js[idx] == j) ||
            (sprs->is[idx] == j && sprs->js[idx] == i)) {
            return idx;
        }
    }
    return -1;
}

void add_spring(Springs *sprs, int i, int j, float rl) {
    if (find_spring(sprs, i, j) != -1) return;

    if (sprs->count >= sprs->capacity) {
        realloc_springs(sprs, sprs->capacity * 2);
    }

    sprs->is[sprs->count] = i;
    sprs->js[sprs->count] = j;

    sprs->rest_lengths[sprs->count] = rl;
    sprs->count++;
}

void shrink_springs(Springs *sprs) {
    if (sprs->count < sprs->capacity / 4 && sprs->capacity > MIN_CAPACITY) {
        int new_capacity = sprs->capacity / 2;
        if (new_capacity < MIN_CAPACITY) new_capacity = MIN_CAPACITY;
        realloc_springs(sprs, new_capacity);
    }
}

void remove_spring(Springs *sprs, int idx, int i, int j) {
    if (sprs->count < sprs->capacity && find_spring(sprs, i, j) != -1) {
        // fix last springs, that remain hanging

        sprs->is[idx] = sprs->is[sprs->count - 1];
        sprs->js[idx] = sprs->js[sprs->count - 1];

        sprs->rest_lengths[idx] = sprs->rest_lengths[sprs->count - 1];
        sprs->count--;
    }
}

void free_springs(Springs *sprs) {
    free(sprs->is);
    free(sprs->js);
    free(sprs->rest_lengths);
}

void dd_relaxation(ParticleSystem *sys, float dt) {
    for (int i=0; i<N; i++) {
        float density = 0.0;
        float near_density = 0.0;

        for (int j=0; j<N; j++) {
            if (i == j) continue;

            float dist_len = length(diff(sys->positions[i], sys->positions[j]));

            if (dist_len < H) {
                density += pow(1 - dist_len / H, 2);
                near_density += pow(1 - dist_len / H, 3);
            }
        }

        float pressure = K * (density - D_0);
        float near_pressure = K_NEAR * near_density;

        Vector2 d_pos = {0.f, 0.f};

        for (int j=0; j<N; j++) {
            if (i == j) continue;

            Vector2 dist = diff(sys->positions[i], sys->positions[j]);
            float dist_len = length(dist);

            if (dist_len < H && dist_len > 1e-4f) {
                float factor = pow(dt, 2) * (pressure * (1 - dist_len / H) +
                        near_pressure * pow(1 - dist_len / H, 2));

                Vector2 versor = scalar_mult(dist, 1.f / dist_len);
                Vector2 displacement = scalar_mult(versor, factor);

                sys->positions[j] = sum(sys->positions[j], scalar_mult(displacement, 0.5f));
                d_pos = diff(d_pos, scalar_mult(displacement, 0.5f));
            }
        }

        sys->positions[i] = sum(sys->positions[i], d_pos);
    }
}

void apply_spring(ParticleSystem *sys, Springs *sprs, float dt) {
    for (int idx = 0; idx < sprs->count; idx++) {
        int i = sprs->is[idx];
        int j = sprs->js[idx];

        Vector2 dist = diff(sys->positions[i], sys->positions[j]);
        float dist_len = length(dist);
        Vector2 versor = scalar_mult(dist, 1.f / length(dist));

        if (dist_len < H) {
            float L = sprs->rest_lengths[idx];

            float factor = (1 - L / H) * (L - dist_len);
            Vector2 displacement = scalar_mult(versor, pow(dt, 2) * K_SPRING * factor);

            sys->positions[i] = diff(sys->positions[i], scalar_mult(displacement, 0.5f));
            sys->positions[j] =  sum(sys->positions[j], scalar_mult(displacement, 0.5f));
        }
    }
}

void adjust_spring(ParticleSystem *sys, Springs *sprs, float dt) {
    for (int idx = 0; idx < sprs->count; idx++) {
        int i = sprs->is[idx];
        int j = sprs->js[idx];

        Vector2 dist = diff(sys->positions[i], sys->positions[j]);
        float dist_len = length(dist);
        Vector2 versor = scalar_mult(dist, 1.f / length(dist));

        if (dist_len < H) {
            if (find_spring(sprs, i, j) == -1) {
                add_spring(sprs, i, j, H);
            }

            float L = sprs->rest_lengths[idx];
            float deformation = L * GAMMA;

            if (dist_len > L + deformation) {
                sprs->rest_lengths[idx] += dt * ALPHA * (dist_len - L - deformation);
            }

            else if (dist_len < L - deformation) {
                sprs->rest_lengths[idx] -= dt * ALPHA * (L - deformation - dist_len); 
            }
        }
    }

    for (int idx = sprs->count - 1; idx >= 0; idx--) {
        if (sprs->rest_lengths[idx] > H) {
            remove_spring(sprs, idx, sprs->is[idx], sprs->js[idx]);
        }
    }
}

void viscosity(ParticleSystem *sys, float dt) {
    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++) {
            if (i == j) continue;
            if (i > j) break;

            Vector2 dist = diff(sys->positions[i], sys->positions[j]);
            Vector2 versor = scalar_mult(dist, 1.f / length(dist));

            if (length(dist) < H) {
                float factor = length(sys->velocities[i]) - length(sys->velocities[j]);
                float lu = length(scalar_mult(versor, factor));

                if (lu > 0) {
                    float li = dt * (1 - length(dist) / H) * (SIGMA * lu + BETA * lu * lu);
                    Vector2 half_impulse = scalar_mult(versor, li * 0.5f);

                    sys->velocities[i] = diff(sys->velocities[i], half_impulse);
                    sys->velocities[j] =  sum(sys->velocities[j], half_impulse);
                }
            }
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
    alloc_springs(&sprs, 128); // memory inefficient, fix later

    int frames = 0;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        BeginDrawing();
        ClearBackground(BLACK);

        for (int i=0; i<N; i++) {
            // gravity
            sys.velocities[i] = sum(scalar_mult(g, dt), sys.velocities[i]);
        }

        viscosity(&sys, dt);

        for (int i=0; i<N; i++) {
            sys.prev_positions[i] = sys.positions[i];
            sys.positions[i] = sum(scalar_mult(sys.velocities[i], dt), sys.positions[i]);
        }

        adjust_spring(&sys, &sprs, dt);
        apply_spring(&sys, &sprs, dt);

        dd_relaxation(&sys, dt);

        for (int i=0; i<N; i++) {
            sys.velocities[i] = calc_next_vel(sys.positions[i], sys.prev_positions[i], dt);
            borders(&sys.positions[i], &sys.velocities[i]);

            DrawText(TextFormat("%0.3f", 1.f / dt), 10, 10, 40, WHITE);
            DrawCircleV(sys.positions[i], R, color);
        }

        EndDrawing();

        if (++frames % FPS == 0) {
            shrink_springs(&sprs);
        }
    }

    CloseWindow();

    free_system(&sys);
    free_springs(&sprs);

    return 0;
}
