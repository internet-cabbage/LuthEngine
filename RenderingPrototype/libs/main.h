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

// My library files 😍😍😍😍😍😍
#include "shader.h"
#include "inputOutput.h"
#include "dataImporter.h"


extern float glowCoefficient;


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"