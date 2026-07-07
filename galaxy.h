#ifndef GALAXY_H
#define GALAXY_H

#include "types.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>



int plummerFunc(int *capacity, int NSpawn, int count, double* xVals, double* yVals, double* zVals, double* vxVals, double* vyVals, double* vzVals, vec3 xOff, vec3 vOff, double aConst, double G, double* MVals, double maxR);
 
int distributionFunction(int *N, int NSpawn, int count, double maxR, double rotSpeed, int genType, 
                        double* xVals, double* yVals, double* zVals,
                        double* vxVals, double* vyVals, double* vzVals,
                        vec3 xOff, vec3 yOff);

double* randomGen(int lower, int upper, unsigned int N);
double* randomContinuousPositive(double maxMag, int N);
double* randomContinuous(double maxMag, int N);

#endif