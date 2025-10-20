#include "physics.h"

static void dd_relaxation(ParticleSystem*, float);
static void apply_spring(ParticleSystem*, Springs*, float);
static void adjust_spring(ParticleSystem*, Springs*, float);
static void viscosity(ParticleSystem*, float);

static Vector2 calc_next_vel(Vector2, Vector2, float);
static void borders(Vector2*, Vector2*);


void update(ParticleSystem *sys, Springs *sprs, int *frames, float dt) {
    for (int i = 0; i < N; i++) {
        // gravity
        sys->velocities[i] = sum(scalar_mult(g, dt), sys->velocities[i]);
    }

    viscosity(sys, dt);

    for (int i = 0; i < N; i++) {
        sys->prev_positions[i] = sys->positions[i];
        sys->positions[i] = sum(scalar_mult(sys->velocities[i], dt), sys->positions[i]);
    }

    adjust_spring(sys, sprs, dt);
    apply_spring(sys, sprs, dt);

    dd_relaxation(sys, dt);

    for (int i = 0; i < N; i++) {
        sys->velocities[i] = calc_next_vel(sys->positions[i], sys->prev_positions[i], dt);
        borders(&sys->positions[i], &sys->velocities[i]);

        DrawText(TextFormat("%0.1f", 1.f / dt), 10, 10, 40, WHITE);
        DrawCircleV(sys->positions[i], R, color);
    }

    if (++(*frames) % FPS == 0) {
        shrink_springs(sprs);
    }
}

static void dd_relaxation(ParticleSystem *sys, float dt) {
    for (int i = 0; i < N; i++) {
        float density = 0.0;
        float near_density = 0.0;

        for (int j = 0; j < N; j++) {
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

        for (int j = 0; j < N; j++) {
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

static void apply_spring(ParticleSystem *sys, Springs *sprs, float dt) {
    for (int idx = 0; idx < sprs->count; idx++) {
        int i = sprs->is[idx];
        int j = sprs->js[idx];

        Vector2 dist = diff(sys->positions[i], sys->positions[j]);
        float dist_len = length(dist);

        if (dist_len < H && dist_len > 1e-4f) {
            Vector2 versor = scalar_mult(dist, 1.f / length(dist));
            float L = sprs->rest_lengths[idx];

            float factor = (1 - L / H) * (L - dist_len);
            Vector2 displacement = scalar_mult(versor, pow(dt, 2) * K_SPRING * factor);

            sys->positions[i] = diff(sys->positions[i], scalar_mult(displacement, 0.5f));
            sys->positions[j] =  sum(sys->positions[j], scalar_mult(displacement, 0.5f));
        }
    }
}

static void adjust_spring(ParticleSystem *sys, Springs *sprs, float dt) {
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            Vector2 dist = diff(sys->positions[i], sys->positions[j]);
            float dist_len = length(dist);

            if (dist_len < H && dist_len > 1e-4f) {
                int idx = find_spring(sprs, i, j);

                if (idx == -1) {
                    add_spring(sprs, i, j, dist_len);
                } 

                else {
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
        }
    }

    for (int idx = sprs->count - 1; idx >= 0; idx--) {
        if (sprs->rest_lengths[idx] >= SPRING_THRESHOLD * H) {
            remove_spring(sprs, idx);
        }
    }
}

static void viscosity(ParticleSystem *sys, float dt) {
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            Vector2 dist = diff(sys->positions[i], sys->positions[j]);
            float dist_len = length(dist);

            if (dist_len < H && dist_len > 1e-4f) {
                Vector2 versor = scalar_mult(dist, 1.f / length(dist));
                float lu = length(sys->velocities[i]) - length(sys->velocities[j]);

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

static void borders(Vector2 *pos, Vector2 *vel) {
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

static Vector2 calc_next_vel(Vector2 pos, Vector2 prev_pos, float dt) {
    Vector2 pos_diff = diff(pos, prev_pos);
    Vector2 next_vel = scalar_mult(pos_diff, 1.0f / dt);
    
    if (isnan(next_vel.x) || isnan(next_vel.y)) {
        return (Vector2){0, 0};
    }
    
    return next_vel;
}
