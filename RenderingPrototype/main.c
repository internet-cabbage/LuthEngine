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


    unsigned int starShaderProgram = linkShaders("vertexShader.vs", "fragmentShader.fs");

    // Import star data
    char filePath[] = "/Users/luthaisb/Code/C++/Galaxy_Simulation_3D_Refactor/DataOutput.bin";
    importPreChecks(&filePath[0]);
    vec3* starPositions = importStarFrame(&filePath[0], 0);
    int starIndex = 0; // The frame to start the animation at
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    if (N == -1 || tSteps == -1) {
        perror("ERROR: Star parameter data was unable to be imported");
        exit(-1);
    }

    // =============================================
    // Buffer object
    // =============================================
    unsigned int starVBO;
    glGenBuffers(1, &starVBO);
    glBindBuffer(GL_ARRAY_BUFFER, starVBO);
    glBufferData(GL_ARRAY_BUFFER, N * 3 * sizeof(float),starPositions, GL_DYNAMIC_DRAW);

    unsigned int starVAO;
    glGenVertexArrays(1, &starVAO);
    glBindVertexArray(starVAO);

    // Configure vertex data
    
    GLint starShaderIndex = glGetAttribLocation(starShaderProgram, "starPos"); // locates where "starPos" is actually held
    glEnableVertexAttribArray(starShaderIndex); // Enables the use of this attribute
    glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE,3*sizeof(float),(void*) 0); // Defines the size of the data
    glVertexAttribDivisor(starShaderIndex, 1);
    float DEG2RAD = M_PI / 180.0f;
    

    // Find matrix uniform locations

    int projectionLocation = glGetUniformLocation(starShaderProgram, "projection");
    int viewLocation = glGetUniformLocation(starShaderProgram, "view");
    int radiusLocation = glGetUniformLocation(starShaderProgram, "radius");
    int colourLocation = glGetUniformLocation(starShaderProgram, "starColour");
    int glowScaleLocation = glGetUniformLocation(starShaderProgram, "glowScale");

    vec3 starWorldPosition = {0.0,0.0,0.0};
    float starRadius = 5.0;
    // Find right vector
    vec3 cameraRight;
    glm_vec3_cross(cameraUp, cameraFront, cameraRight);
    glm_normalize(cameraRight);

    // Sets the callback functions which accepts users key input and camera input
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window,mouseCallback);

    if (projectionLocation == -1 || viewLocation == -1 || radiusLocation == -1 || colourLocation == -1 || glowScaleLocation == -1) {
        printf("ERROR: One or more of the uniform locations was unable to be successfully found \n");
            printf("\nIf any location is -1, an error in locating the uniforms has occured: \n\n\tProjection location: %d\n\tview location: %d\n\tradius location: %d \n\tcolour location %d\n\tnglowScale location: %d\n", projectionLocation, viewLocation, radiusLocation, colourLocation, glowScaleLocation);
        exit(-1);
    }


    float outputTime = glfwGetTime();
    // render loop
    glDepthFunc(GL_NEVER);
    //glEnable(GL_DEPTH_TEST);
    //glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ONE);
    while(!glfwWindowShouldClose(window)) {
        // Delta timing
        float time = (float) glfwGetTime();
        deltaTime = time - lastFrameTime;
        lastFrameTime = time;

        if (time - outputTime > 1.0f) {
            fprintf(stdout, "FPS: (%f) \n", 1.0/deltaTime);
            outputTime = time;
            fflush(stdout);
        }
        processInput(window);

        // Camera input handling
        if (mouseCaptured == true) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
              
        // Resize handling
        int winWidth, winHeight;
        glfwGetFramebufferSize(window,&winWidth,&winHeight);

        glClearColor(0.0,0.0,0.0,0.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        glUseProgram(starShaderProgram);

        // Update star positions
        if (starIndex < tSteps) {
            //fprintf(stdout,"Update function run, starIndex: %d \n", starIndex);
            //fflush(stdout);
            //starIndex++;
        }
        else {
            starIndex = 1;
            printf("Reset");
            fflush(stdout);
        }
        free(starPositions);
        starPositions = importStarFrame(&filePath[0], starIndex);
        glBufferSubData(GL_ARRAY_BUFFER,0,N*3*sizeof(float),starPositions);

        
        
        


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

        vec4 starWorldV4 = {starWorldPosition[0],starWorldPosition[1],starWorldPosition[2],1.0};
        vec4 starCam; // The stars position in camera space
        glm_mat4_mulv(view,starWorldV4,starCam);
        glUniformMatrix4fv(projectionLocation,1,GL_FALSE,(float*)projection);
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, (float*)view);
        glUniform1f(radiusLocation,starRadius);
        glUniform3f(colourLocation, 1.0f,0.95f,0.8f);
        glUniform1f(glowScaleLocation, 3.0);

        glBindVertexArray(starVAO);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP,0,4,N);
    
        // Swap buffers and check for events.
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}