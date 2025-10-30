#include "kernels/consts.cl"
#include "kernels/vect_operations.cl"

void dd_relaxation(int i, __global ParticleSystem *sys, int N, float dt) {
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

    vector2 d_pos = {0.f, 0.f};

    for (int j = 0; j < N; j++) {
        if (i == j) return;

        vector2 dist = diff(sys->positions[i], sys->positions[j]);
        float dist_len = length(dist);

        if (dist_len < H && dist_len > 1e-4f) {
            float factor = pow(dt, 2) * (pressure * (1 - dist_len / H) +
                    near_pressure * pow(1 - dist_len / H, 2));

            vector2 versor = scalar_mult(dist, 1.f / dist_len);
            vector2 displacement = scalar_mult(versor, factor);

            sys->positions[j] = sum(sys->positions[j], scalar_mult(displacement, 0.5f));
            d_pos = diff(d_pos, scalar_mult(displacement, 0.5f));
        }
    }

    sys->positions[i] = sum(sys->positions[i], d_pos);
}

vector2 calc_next_vel(vector2 pos, vector2 prev_pos, float dt) {
    vector2 pos_diff = diff(pos, prev_pos);
    vector2 next_vel = scalar_mult(pos_diff, 1.0f / dt);
    
    if (isnan(next_vel.x) || isnan(next_vel.y)) {
        return (vector2){0, 0};
    }
    
    return next_vel;
}
