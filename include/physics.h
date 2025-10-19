#ifndef PHYSICS_H
#define PHYSICS_H

#include "vect.h"
#include "springs.h"

void dd_relaxation(ParticleSystem*, float);
void apply_spring(ParticleSystem*, Springs*, float);
void adjust_spring(ParticleSystem*, Springs*, float);
void viscosity(ParticleSystem*, float);

#endif
