#ifndef SIM_H
#define SIM_H

#include "types.h"
#include "stdlib.h"


extern double buildTime;
extern double forceTime;

void timeLoop(body *bodies, size_t N, double antiSingularity, double G, double dt, double theta, int xmax, int ymax, int zmax);

#endif

