#include "consts.h"

// screen dimensions
const int X = 1000;
const int Y = 800;
const int FPS = 60;

// name of kernel file
const char *KERNEL_FILENAME = "kernel.cl";

// particle system
const int N = 1000;
const int MIN_CAPACITY = 64;
const float R = 4.f;
const Color color = GRAY;

// physics constants
const float G = 9.81f;
const Vector2 g = {0, G};
const float DAMPING = 0.7f;

// SPH parameters
const float H = 20.f;           // interaction radius
const float D_0 = 40.f;         // rest density
const float K = 30.f;           // stiffness constant
const float K_NEAR = 60.f;      // near stiffness constant
const float K_SPRING = 30.f;    // spring constant for elasticity
const float SPRING_THRESHOLD = 0.7f;  // resize H as threshold

// plasticity constants
const float GAMMA = 0.1f;       // yield ratio
const float ALPHA = 0.2f;       // plasticity constant
const float SIGMA = 0.2f;       // first viscosity constant
const float BETA = 0.2f;        // second viscosity constant
