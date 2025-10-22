#ifndef CONSTS_CL
#define CONSTS_CL

typedef struct {
    float x;
    float y;
} vector2;

// screen dimensions
__constant int X = 1000;
__constant int Y = 800;
__constant int FPS = 60;
__constant float dt = 1.f / FPS;

// particle system
__constant int N = 1000;
__constant int MIN_CAPACITY = 64;
__constant float R = 4.f;

// physics constants
__constant vector2 g = {0, 9.81f};
__constant float DAMPING = 0.7f;

// SPH parameters
__constant float H = 20.f;           // interaction radius
__constant float D_0 = 40.f;         // rest density
__constant float K = 30.f;           // stiffness constant
__constant float K_NEAR = 60.f;      // near stiffness constant
__constant float K_SPRING = 30.f;    // spring constant for elasticity
__constant float SPRING_THRESHOLD = 0.7f;  // resize H as threshold

// plasticity constants
__constant float GAMMA = 0.1f;       // yield ratio
__constant float ALPHA = 0.2f;       // plasticity constant
__constant float SIGMA = 0.2f;       // first viscosity constant
__constant float BETA = 0.2f;        // second viscosity constant

typedef struct {
    vector2 *positions;
    vector2 *prev_positions;
    vector2 *velocities;
} ParticleSystem;

typedef struct {
    int *is;
    int *js;
    float *rest_lengths;
    int count;
    int capacity;
} Springs;

#endif
