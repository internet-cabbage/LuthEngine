#include "graphics.h"


// A function which initialises all the settings required to render the stars
void starRenderInitialisation(unsigned int starShaderProgram, unsigned int HDRFrameBufferObject) {
    glUseProgram(starShaderProgram);
    glEnable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER,HDRFrameBufferObject);
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT); glClear(GL_DEPTH_BUFFER_BIT);
}


void postProcessing(unsigned int postProcessingShaderProgram, unsigned int hdrColorBuffer, int hdrBufferLocation, int exposureLocation, float exposure, unsigned int HDRVAO) {
    // Unbind the previous framebuffer
    glUseProgram(postProcessingShaderProgram);
    glDisable(GL_BLEND);
    // The default framebuffer of 0 is set to clamp intensity values between the range [0,1]
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glClear(GL_COLOR_BUFFER_BIT); glClear(GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f,1.0f,1.0f,1.0f);

    glBindVertexArray(HDRVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrColorBuffer);
    glUniform1i(hdrBufferLocation,0);

    glUniform1f(exposureLocation,1.0f);
    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}

// This has a lot of comments as in all honestly I only partially understand the code
// I have made use of many different internet tutorials as well as a lot of stack overflow

    // =============================================
    // HDR Custom framebuffer
    // =============================================
    // Source: https://www.cosmiclearn.com/opengl/hdr.php
    /*
    The standard framebuffer stores intensity values in the range [0.0,1.0], but the fragment shader is able to produce any intensity in the range [0.0,infinity].
    This results in very bright objects all being rendered as the same colour (pure white), causing a lot of detail to be lost.
    To fix this, we can store the intensities in a custom framebuffer, then perform a post-processing path in order to
    transform the infinite range of intensities onto the finite range supported by the screen.
    */
void createHDRFramebuffer(GLFWwindow* window, unsigned int postProcessingShaderProgram, unsigned int* HDRFBO, unsigned int* HDRColorBuffer, unsigned int* HDRVAO) {
    int winWidth, winHeight;
    glfwGetFramebufferSize(window, &winWidth, &winHeight);
    
    // The value used to refer to the texture used for the HDR rendering
    glGenTextures(1, HDRColorBuffer); // Asks OpenGL kindly to generate an object ID for a texture and store it in the HDRColorBuffer variable
    glBindTexture(GL_TEXTURE_2D, *HDRColorBuffer); // Makes the texture stored in HDRColorBuffer the current active texture

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16F,winWidth,winHeight,0,GL_RGBA,GL_FLOAT,NULL); // This sets the properties of the image such that it will cover the whole screen
    // Additionally the image is given a floating point RGB value that supports values greater than 1.0, so it will be used to store the unprocessed HDR data.

    // Filtering info for texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    // An ID used to refer to the custom framebuffer
    glGenFramebuffers(1, HDRFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, *HDRFBO);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,*HDRColorBuffer,0); // Attaches the HDR supporting texture to this framebuffer

    // Add a depth buffer to the framebuffer, so depth testing can be used

    unsigned int depthBufferObject;
    glGenRenderbuffers(1,&depthBufferObject);
    glBindRenderbuffer(GL_RENDERBUFFER,depthBufferObject);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,winWidth,winHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthBufferObject);

    // Check to see if the frameBuffer was produced correctly

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("ERROR: Framebuffer object was not able to be created.\n");
        fflush(stdout);
    }

    glBindFramebuffer(GL_FRAMEBUFFER,0);

    // Now we have just finished producing the frameBuffer
    // to complete this, we need to actually store the vertices of the screen covering texture in the GPU


    float HDRSquare[] = {
        -1.0,-1.0,0.0,
        1.0,-1.0,0.0,
        -1.0,1.0,0.0,
        1.0,1.0,0.0
    };

    unsigned int HDRVBO;
    glGenBuffers(1, &HDRVBO);
    glBindBuffer(GL_ARRAY_BUFFER, HDRVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(HDRSquare),HDRSquare,GL_STATIC_DRAW);

    // We have stored the vertex position data in memory, so now we have to tell the GPU what attributes are represented by which bytes
    //unsigned int HDRVAO;
    glGenVertexArrays(1, HDRVAO);
    glBindVertexArray(*HDRVAO);

    GLint HDRPositionIndex = glGetAttribLocation(postProcessingShaderProgram, "aPos");
    glEnableVertexAttribArray(HDRPositionIndex);
    glVertexAttribPointer(HDRPositionIndex,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
}

void tempToRGB(float temp, ivec3* RGBOut) {
    temp *= 0.01;
    float red, green, blue;

    // Red values
    if (temp > 66) {
        red = 255.0f;
    }
    else {
        red = temp - 60.0;
        red = 329.7 * pow(red,-0.1332);
        if (red < 0) {red = 0;}
        if (red > 255) {red = 255;}
    }

    // Green values

    if (temp < 66)
    {
        green = temp;
        green = 99.47 * log(green) - 161.12;
        if (green < 0) {green = 0;}
        if (green > 255) {green = 255;}
    }
    else {
        green = temp - 60;
        green = 288.12 * pow(green, -0.0755);
        if (green < 0) {green = 0;}
        if (green > 255) {green = 255;}
    }

    // Blue values

    if (temp > 65) {blue = 255;}
    else {
        if (temp < 19) {blue = 0;}
        else {
            blue = temp - 10;
            blue = 138.5 * log(blue) - 305;
            if (blue < 0) {blue = 0;}
            if (blue > 255) {blue = 255;}
        }
    }

    // I am using an unsigned char to represent the integer as it saves a lot of memory.
    (*RGBOut)[0] = (unsigned char)red;
    (*RGBOut)[1] = (unsigned char)green;
    (*RGBOut)[2] = (unsigned char)blue;
}