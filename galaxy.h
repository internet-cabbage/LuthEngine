#ifndef GALAXY_H
#define GALAXY_H

#include "types.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>



size_t plummerFunc(size_t *capacity, size_t NSpawn, size_t count, double* xVals, double* yVals, double* zVals, double* vxVals, double* vyVals, double* vzVals, vec3 xOff, vec3 vOff, double aConst, double G, double* MVals, double maxR, RGB* colourVals);
 
size_t distributionFunction(size_t *N, size_t NSpawn, size_t count, double maxR, double rotSpeed, int genType, 
                        double* xVals, double* yVals, double* zVals,
                        double* vxVals, double* vyVals, double* vzVals,
                        vec3 xOff, vec3 yOff, RGB* colourVals);

double* randomGen(int lower, int upper, size_t N);
double* randomContinuousPositive(double maxMag, size_t N);
double* randomContinuous(double maxMag, size_t N);

#endif