#include "physics.h"

void dd_relaxation(ParticleSystem *sys, float dt) {
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

void apply_spring(ParticleSystem *sys, Springs *sprs, float dt) {
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

void adjust_spring(ParticleSystem *sys, Springs *sprs, float dt) {
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
        // int i = sprs->is[idx];
        // int j = sprs->js[idx];
        
        // float dist_len = length(diff(sys->positions[i], sys->positions[j]));
        
        if (sprs->rest_lengths[idx] >= SPRING_THRESHOLD * H) {
            remove_spring(sprs, idx);
        }
    }
}

void viscosity(ParticleSystem *sys, float dt) {
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
