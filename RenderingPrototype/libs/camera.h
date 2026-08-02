#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <cglm/cglm.h>
#include "inputOutput.h"

// Camera physical variables
extern vec3 cameraPos;
extern vec3 cameraFront;
extern vec3 cameraUp;
extern float cameraFOV;

// Camera movement variables
extern float sensitivity;
extern float cameraSpeed;
extern float maxCamSpeed;
extern float minCamSpeed;
extern float rollSpeed;

// Rendering variables
extern float maxRenderDistance;
extern float minRenderDistance;


void yawCam(float yaw);
void pitchCam(float pitch);
void rollCam(float roll);


#endif