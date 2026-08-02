#ifndef SHADER_H
#define SHADER_H

#include <limits.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 



unsigned int linkShaders(char* vertexShaderName, char* fragmentShaderName);

//unsigned int readTexture(char* imageName, int textureNo);
char* findTextureLocation(char* imageName);
#endif