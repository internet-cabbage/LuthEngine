#include "libs/main.h"

float* randomArray(int arrayLength, float maxSize) {
    float* randVals = calloc(arrayLength,sizeof(float));
    for (int i = 0; i < arrayLength; i++) {
        if ((float)rand()/(float)RAND_MAX > 0.5) {
            randVals[i] = (float)rand()/(float)(RAND_MAX) * maxSize;
        }
        else {
            randVals[i] = - (float)rand()/(float)(RAND_MAX) * maxSize;
        }
    }
    return randVals;
}


void timingFunctions(void) {
    // Delta timing
    float time = (float) glfwGetTime();
    deltaTime = time - lastFrameTime;
    lastFrameTime = time;

    if (time - outputTime > 1.0f) {
        fprintf(stdout, "FPS: (%f) \n", 1.0/deltaTime);
        outputTime = time;
        fflush(stdout);
    }
}

void focusHandler(GLFWwindow* window){
    if (mouseCaptured == true) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}




int main(void) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    srand(time(0));
    int screenWidth, screenHeight, screenXPos, screenYPos;

    glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(),&screenXPos,&screenYPos,&screenWidth,&screenHeight);
    printf("Screen size (%d x %d) \n", screenWidth, screenHeight);
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Luthais's test", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialise GLAD");
        return -1;
    }

    // This tells glfw to call the resize function when the screen is resized
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);  

    // Framebuffer size
    // This is needed as for an apple device each physical pixel is described by 4 pixel values for some reason
    int winWidth, winHeight;
    glfwGetFramebufferSize(window, &winWidth, &winHeight);


    unsigned int starShaderProgram = linkShaders("vertexShader.vs", "fragmentShader.fs");
    unsigned int postProcessingShaderProgram = linkShaders("hdrVertexShader.vs", "hdrGamma.fs");

    // Import star data
    char filePath[] = "/Users/luthaisb/Code/C++/Galaxy_Simulation_3D_Refactor/DataOutput.bin";
    importPreChecks(&filePath[0]);
    vec3* starPositions = importStarFrame(&filePath[0], 0);
    starIndex = 0; // The frame to start the animation at
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    if (N == -1 || tSteps == -1) {
        perror("ERROR: Star parameter data was unable to be imported");
        exit(-1);
    }


    unsigned int HDRFBO, HDRColorBuffer, HDRVAO;
    createHDRFramebuffer(window, postProcessingShaderProgram, &HDRFBO,&HDRColorBuffer, &HDRVAO);

    // =============================================
    // Buffer objects
    // =============================================

    // Stores the vertex array object which defines what the VBO bytes represent
    unsigned int starVAO;
    glGenVertexArrays(1, &starVAO);
    glBindVertexArray(starVAO);

    // Add star position data to the VAO
    GLint stridePosVBO = 3 * sizeof(float);
    GLint starPosIndex = -1;
    starPosIndex = glGetAttribLocation(starShaderProgram, "starPos"); // locates where "starPos" is actually held
    if (starPosIndex == -1) { // Basic error checking
        printf("Error finding the star position attribute location");
        fflush(stdout);
        exit(-1);
    }

    // VBO storing the position data for the stars
    unsigned int starVBO;
    glGenBuffers(1, &starVBO);
    glBindBuffer(GL_ARRAY_BUFFER, starVBO);
    glBufferData(GL_ARRAY_BUFFER, N * 3 * sizeof(float),starPositions, GL_DYNAMIC_DRAW);

    // Configuring the VAO to accept the position attributes
    glEnableVertexAttribArray(starPosIndex); // Enables the use of this attribute
    glVertexAttribPointer(starPosIndex, 3, GL_FLOAT,GL_FALSE,stridePosVBO,(void*) 0); // Defines the size of the data
    glVertexAttribDivisor(starPosIndex, 1);
    
    // =================================
    // Colour data
    GLint strideColorVBO = sizeof(RGB);
    GLint starColourIndex = -1;
    starColourIndex = glGetAttribLocation(starShaderProgram, "starUniqueColour");
    if (starColourIndex == -1) {
        printf("Star colour attribute location could not be found");
        fflush(stdout);
        //exit(-1);
    }

    // VBO storing the star colour
    unsigned int starColourVBO;
    glGenBuffers(1, &starColourVBO);
    glBindBuffer(GL_ARRAY_BUFFER, starColourVBO);
    glBufferData(GL_ARRAY_BUFFER, N * sizeof(RGB),colourVals,GL_STATIC_DRAW);

    // Configure the star VAO to accept the colour attributes
    glEnableVertexAttribArray(starColourIndex);
    glVertexAttribPointer(starColourIndex, 3, GL_UNSIGNED_BYTE, GL_TRUE, strideColorVBO,(void*)0);
    glVertexAttribDivisor(starColourIndex,1);



    float DEG2RAD = M_PI / 180.0f;
    

    // Find uniform locations in the starShaderProgram

    int projectionLocation = glGetUniformLocation(starShaderProgram, "projection");
    int viewLocation = glGetUniformLocation(starShaderProgram, "view");
    int radiusLocation = glGetUniformLocation(starShaderProgram, "radius");
    int glowScaleLocation = glGetUniformLocation(starShaderProgram, "glowScale");

    // Find uniform locations in the postProcessingShaderProgram

    int hdrBufferLocation = glGetUniformLocation(postProcessingShaderProgram, "hdrBuffer");
    int exposureLocation = glGetUniformLocation(postProcessingShaderProgram, "exposure");

    float starRadius = 3.0;

    // Sets the callback functions which accepts users key input and camera input
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window,mouseCallback);

    if (projectionLocation == -1 || viewLocation == -1 || radiusLocation == -1 || glowScaleLocation == -1 || hdrBufferLocation == -1 || exposureLocation == -1) {
        printf("ERROR: One or more of the uniform locations was unable to be successfully found \n");
            printf("\nIf any location is -1, an error in locating the uniforms has occured: \n\n\tProjection location: %d\n\tview location: %d\n\tradius location: %d \n\tglowScale location: %d\n\thdrBufferLocation: %d\n\texposureLocation: %d", projectionLocation, viewLocation, radiusLocation, glowScaleLocation, hdrBufferLocation, exposureLocation);
        exit(-1);
    }


    outputTime = glfwGetTime();
    // render loop
    glDepthFunc(GL_NEVER);
    //glEnable(GL_DEPTH_TEST);
    //glDisable(GL_DEPTH_TEST);

    vec3 cameraRight;
    

    glBlendFunc(GL_ONE,GL_ONE);
    while(!glfwWindowShouldClose(window)) {
        // Function which handles delta timing as well as printing the FPS
        timingFunctions();
        processInput(window);

        // Camera input handling
        focusHandler(window);

        starRenderInitialisation(starShaderProgram, HDRFBO);
        // Check if the rendering is paused
        if (isPaused == false) {
            // Update star positions
            if (starIndex < tSteps) {
                starIndex++;
            }
            else {
                starIndex = 1;
                printf("Reset");
                fflush(stdout);
            }
            free(starPositions);
            starPositions = importStarFrame(&filePath[0], starIndex);
            glBindBuffer(GL_ARRAY_BUFFER,starVBO);
            glBufferSubData(GL_ARRAY_BUFFER,0,N*3*sizeof(float),starPositions);
        }
        if (isPaused == true && (starIndexOnPause != starIndex)) {
            free(starPositions);
            starPositions = importStarFrame(&filePath[0], starIndex);
            glBindBuffer(GL_ARRAY_BUFFER,starVBO);
            glBufferSubData(GL_ARRAY_BUFFER,0,N*3*sizeof(float),starPositions);
        }



        // ==================================================
        // Camera updates
        // ==================================================

        mat4 view, projection;

        // Find right vector
        glm_vec3_cross(cameraUp, cameraFront, cameraRight);
        glm_normalize(cameraRight);

        // Find camera up vector
        glm_vec3_cross(cameraFront, cameraRight, cameraUp);
        glm_normalize(cameraUp);

        // Lookat matrix

        vec3 cameraTarget;
        glm_vec3_add(cameraPos,cameraFront,cameraTarget);
        glm_lookat(cameraPos,cameraTarget,cameraUp,view);
        // Update matrix uniforms

        glm_perspective(DEG2RAD * cameraFOV,(float)winWidth/(float)winHeight,minRenderDistance,maxRenderDistance,projection);
      
        // Star rendering:

        glUniformMatrix4fv(projectionLocation,1,GL_FALSE,(float*)projection);
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, (float*)view);
        glUniform1f(radiusLocation,starRadius);
        glUniform1f(glowScaleLocation, 5.0);

        glBindVertexArray(starVAO);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP,0,4,N);

        // ==================================================
        // Post processing
        // ==================================================

        postProcessing(postProcessingShaderProgram,HDRColorBuffer,hdrBufferLocation,exposureLocation,1.0f,HDRVAO);
        // Unbind the custom framebuffer
   
    
        // Swap buffers and check for events.
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
