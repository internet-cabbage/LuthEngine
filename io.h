#ifndef IO_H
#define IO_H

#include "types.h"
#include <stdio.h>

void writeFrame(FILE *datafile, body *bodies, size_t N, float *frameBuffer);
void printProgress(int stepVal, int tSteps, int width);

#endif