#include "camera.h"

// Camera globals
vec3 cameraPos =    {0.0,0.0,3.0};
vec3 cameraFront =  {0.0,0.0,-1.0};
vec3 cameraUp =     {0.0,1.0,0.0};
float cameraFOV =   45.0f;

float maxRenderDistance = 20000.0f;
float minRenderDistance = 0.1f;
float maxCamSpeed = 2000.0f;
float minCamSpeed = 5.0f;
float cameraSpeed = 200.f;

// Mouse globals
float sensitivity = 0.001f;
float rollSpeed = 0.015f;



// Rotate the camera about the axis of its right vector
void pitchCam(float pitch) {
    vec3 rightVec;
    glm_vec3_cross(cameraFront,cameraUp,rightVec);
    glm_vec3_norm(rightVec);
    glm_vec3_rotate(cameraFront,pitch,rightVec);
    glm_vec3_rotate(cameraUp,pitch,rightVec);
}
// Rotate the camera about the axis of its up vector
void yawCam(float yaw) {
    glm_vec3_rotate(cameraFront,yaw,cameraUp);
}
void rollCam(float roll) {
    glm_vec3_rotate(cameraUp,roll, cameraFront);
}