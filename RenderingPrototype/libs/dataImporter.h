#ifndef DATAIMPORTER_H
#define DATAIMPORTER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <limits.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

extern int N;
extern int tSteps;

vec3* importStarFrame(char* filePath, int frameIndex);
void importPreChecks(char* filePath);

#endif