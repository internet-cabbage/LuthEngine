#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <mach-o/dyld.h>
#include <time.h>
#include <math.h>
#include <cglm/cglm.h>

void tempToRGB(float temp, ivec3* RGBOut);

void starRenderInitialisation(unsigned int starShaderProgram, unsigned int HDRFrameBufferObject);

void postProcessing(unsigned int postProcessingShaderProgram, unsigned int hdrColorBuffer, int hdrBufferLocation, int exposureLocation, float exposure, unsigned int HDRVAO);

void createHDRFramebuffer(GLFWwindow* window, unsigned int postProcessingShaderProgram, unsigned int* HDRFBO, unsigned int* HDRColorBuffer, unsigned int* HDRVAO);

#endif