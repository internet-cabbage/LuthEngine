#ifndef INPUTOUTPUT_H
#define INPUTOUTPUT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <mach-o/dyld.h>
#include <time.h>
#include <cglm/cglm.h>
#include "camera.h"



// Mouse globals
extern float lastX;
extern float lastY;
extern bool mouseCaptured;
extern bool mouseSwitchedState;

// Delta timing globals
extern float deltaTime;
extern float lastFrameTime;

// Function declarations

void mouseCallback(GLFWwindow* window, double xPos, double yPos);
void processInput(GLFWwindow *window);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);  
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);


//Temp
extern float glowCoefficient;

#endif