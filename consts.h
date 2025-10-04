#ifndef CONSTANTS_H
#define CONSTANTS_H

const int X = 1000;
const int Y =  800;
const int FPS = 60;

const int N = 1000;
const int MIN_CAPACITY = 64;

#if 1
const float R = 4.f;
const Color color = GRAY;
#else
const float R = 4.f;
const Color color = {40, 40, 200, 60};
#endif

const float G = 9.81f;
// const float G = 0.f;
const Vector2 g = {0, G};

const float DAMPING = 0.7f;

// interaction radius
const float H = 20.f;

// rest density
const float D_0 = 40.f;

// stiffness constant
const float K = 30.f;

// near stiffness constant
const float K_NEAR = 2.f * K;

// spring constant for elasticity
// const float K_SPRING = 10.f;
const float K_SPRING = 30.f;

// resize H as threshold to remove a spring
const float SPRING_THRESHOLD = 0.7f;

// yield ratio
const float GAMMA = 0.1f;

// plasticity constant
const float ALPHA = 0.2f;

// first viscosity constant
const float SIGMA = 0.2f;

// second viscosity constant
const float BETA = 0.2f;

#endif
