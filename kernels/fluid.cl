#include "kernels/consts.cl"

void borders(vector2 *pos, vector2 *vel) {
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
    __global Springs *s
) {

    int id = get_global_id(0);

    vector2 *vel = &p->velocities[id];

    vel->x += g.x * dt;
    vel->y += g.y * dt;

    p->positions[id].x = p->positions[id].x - vel->x;
    p->positions[id].y = p->positions[id].y + vel->y;

    borders(&p->positions[id], vel);
}
