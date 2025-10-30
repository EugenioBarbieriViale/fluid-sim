#include "kernels/consts.cl"
#include "kernels/vect_operations.cl"
#include "kernels/physics.cl"

void borders(vector2 *pos, vector2 *vel, int X, int Y, float R) {
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

__kernel void computeNextState(
    __global ParticleSystem *sys,
    __global Springs *sprs,
    Constants c
) {

    int i = get_global_id(0);
    if (i >= c.N) return;

    sys->velocities[i] = sum(scalar_mult(g, c.dt), sys->velocities[i]);
    sys->prev_positions[i] = sys->positions[i];
    sys->positions[i] = sum(scalar_mult(sys->velocities[i], c.dt), sys->positions[i]);

    barrier(CLK_GLOBAL_MEM_FENCE);

    dd_relaxation(i, sys, c.N, c.dt);

    barrier(CLK_GLOBAL_MEM_FENCE);

    sys->velocities[i] = calc_next_vel(sys->positions[i], sys->prev_positions[i], c.dt);
    borders(&sys->positions[i], &sys->velocities[i], c.X, c.Y, c.R);
}
