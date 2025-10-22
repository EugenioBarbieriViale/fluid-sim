#include "kernels/consts.cl"
#include "kernels/vect_operations.cl"

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
    __global ParticleSystem *p,
    __global Springs *s,
    Constants c
) {

    int id = get_global_id(0);

    p->velocities[id] = sum(p->velocities[id], scalar_mult(g, c.dt));
    p->positions[id] = sum(p->positions[id], p->velocities[id]);

    borders(&p->positions[id], &p->velocities[id], c.X, c.Y, c.R);
}
