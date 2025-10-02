#ifndef CONSTANTS_H
#define CONSTANTS_H

const int X = 1000;
const int Y =  800;
const int FPS = 60;

const int N =  500;

#if 1
const float R = 7.f;
const Color color = GRAY;
#else
const float R = 30.f;
const Color color = {40, 40, 200, 60};
#endif

const float G = 9.81f;
const Vector2 g = {0, 1.5 * G};

const float DAMPING = 0.95f;

// interaction radius
const float H = 20.f;

// rest density
const float D_0 = 10.f;

// stiffness constant
const float K = 50.f;

// near stiffness constant
const float K_NEAR = 60.f;

// spring constant for elasticity
const float K_SPRING = 10.f;

// rest length of springs
const float L_0 = H / 2.f;

// yield ratio
const float GAMMA = 0.1f;

// plasticity constant
const float ALPHA = 0.5f;

// first viscosity constant
const float SIGMA = 0.f;

// second viscosity constant
const float BETA = 0.5f;

#endif
