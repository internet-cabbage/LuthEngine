#include "inputOutput.h"


// Mouse globals
float lastX = 400;
float lastY = 400;
bool mouseCaptured = true;
bool mouseSwitchedState = true;

// Delta timing globals
float deltaTime =   0.0f;
float lastFrameTime =   0.0f;
float outputTime = 0.0;

// Pause functionality
bool isPaused = false;
int starIndex = 0;
int starIndexOnPause = -1;

void starIndexChanger(int change) {
    if ((starIndex < 0) || (starIndex > tSteps)) {
        starIndex = 0;
    }
    if (starIndex + change > tSteps) {
        starIndex = (starIndex + change) - tSteps;
    }
    else if (starIndex + change < 0) {
        // The frameDifference variable is the number of index positions the starIndex is currently undershooting the array by
        int frameDifference = abs(starIndex + change);
        starIndex = tSteps - frameDifference;
    }
    else {
        printf("Old index: %d\n",starIndex);
        starIndex += change;
        printf("New index: %d\n",starIndex);
        fflush(stdout);
    }
}


// A function to run when detecting discrete keystrokes (or a repeated stroke with a delay between the key being initially held and the action)
// Held keys should have their functionality implemented via the processInput function
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
        printf("Left alt pressed \n");
        mouseCaptured = !mouseCaptured;
        mouseSwitchedState = true;
        fflush(stdout);
    }
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        cameraSpeed *= 1.35;
        glm_clamp(cameraSpeed, minCamSpeed, maxCamSpeed);
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        cameraSpeed *= 0.8;
        glm_clamp(cameraSpeed, minCamSpeed, maxCamSpeed);       
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        isPaused = !isPaused;
    }
    if ((key == GLFW_KEY_PERIOD) && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        starIndexChanger(-1);
    }
    if ((key == GLFW_KEY_COMMA) && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        starIndexChanger(1);
    }
    if ((key == GLFW_KEY_K) && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        starIndexChanger(30);
    }
    if ((key == GLFW_KEY_L) && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        starIndexChanger(-30);
    }
}

// Mouse input function
/*
The xPos and yPos have tom be doubles, as that is the dataType glfw will supply us
*/
void mouseCallback(GLFWwindow* window, double xPos, double yPos){
    if (mouseCaptured == true) {
        // This checks to see if the mouse has just switched states, and if so it adjusts the lastX and LastY values
        // so that they are the current values, which ensures the camera doesn't jerk around when being focused and unfocused
        if(mouseSwitchedState == true) {
            lastX = xPos;
            lastY = yPos;
            mouseSwitchedState = false;
        }

        float deltaX = xPos - lastX;
        float deltaY = yPos - lastY;
        lastX = xPos;
        lastY = yPos;

        deltaX *= sensitivity;
        deltaY *= sensitivity;
        yawCam(-deltaX);
        pitchCam(-deltaY);
    }
}

// A function which is called to detect and process user input
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
    float cameraRealTimeSpeed = cameraSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        vec3 tempFront;
        glm_vec3_scale(cameraFront,cameraRealTimeSpeed,tempFront);
        glm_vec3_add(cameraPos,tempFront,cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        vec3 tempFront;
        glm_vec3_scale(cameraFront,cameraRealTimeSpeed,tempFront);
        glm_vec3_sub(cameraPos,tempFront,cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        vec3 tempRight;
        glm_vec3_cross(cameraFront,cameraUp,tempRight);
        glm_vec3_normalize(tempRight);
        glm_vec3_scale(tempRight,-cameraRealTimeSpeed,tempRight);
        glm_vec3_add(tempRight,cameraPos,cameraPos);
    }
    if (glfwGetKey(window,GLFW_KEY_D) == GLFW_PRESS){
        vec3 tempRight;
        glm_vec3_cross(cameraFront,cameraUp,tempRight);
        glm_vec3_normalize(tempRight);
        glm_vec3_scale(tempRight,cameraRealTimeSpeed,tempRight);
        glm_vec3_add(tempRight,cameraPos,cameraPos);
    }
    if (glfwGetKey(window,GLFW_KEY_SPACE) == GLFW_PRESS){
        vec3 tempUp, tempRight;
        // Define vectors
        glm_vec3_cross(cameraFront,cameraUp,tempRight);
        glm_vec3_normalize(tempUp);

        glm_vec3_cross(cameraFront,tempRight,tempUp);
        glm_vec3_normalize(tempRight);
        
        glm_vec3_scale(tempUp,-cameraRealTimeSpeed,tempUp);
        glm_vec3_add(tempUp,cameraPos,cameraPos);
    }
    if (glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        vec3 tempUp, tempRight;
        // Define vectors
        glm_vec3_cross(cameraFront,cameraUp,tempRight);
        glm_vec3_normalize(tempUp);

        glm_vec3_cross(cameraFront,tempRight,tempUp);
        glm_vec3_normalize(tempRight);
        
        glm_vec3_scale(tempUp,cameraRealTimeSpeed,tempUp);
        glm_vec3_add(tempUp,cameraPos,cameraPos);
    }
    if (glfwGetKey(window,GLFW_KEY_Q) == GLFW_PRESS) {
        rollCam(-rollSpeed);
    }
    if (glfwGetKey(window,GLFW_KEY_E) == GLFW_PRESS) {
        rollCam(rollSpeed);
    }
}

// Function called when the screen is resized.
void framebufferSizeCallback(GLFWwindow* window, int width, int height){    
        glViewport(0, 0, width, height);
    }  

// Code to let me import a tileSheet


// Code to give me the coordinates of the top left cell in a tile sheet

/*
Convention guide:
    - The image location is given by a vec4 with the following values:
    vec4 imageLocation = {leftEdge, rightEdge, topEdge, bottomEdge};
    - The loader assumes all sprite sheets are composed of sprites with no empty indexes
    - The frameIndex variable is zero indexed and starts at 0.
    - The first sprite appears in the top left of the image.
    - The coordinate axis for pixels has an origin in the bottom left.



*/
void findCellCoord(int framesPerRow, int framesPerColumn, int imgWidth, int imgHeight,  int frameIndex, vec4* locationOutput) {
    int rowIndex = frameIndex % framesPerRow;
    int columnIndex = frameIndex / framesPerRow;

    int cellWidth = imgWidth / framesPerRow;
    int cellHeight = imgHeight / framesPerColumn;
    *locationOutput[0] = cellWidth * rowIndex;
    *locationOutput[1] = cellWidth * (rowIndex+1);

    *locationOutput[2] = cellHeight * columnIndex;
    *locationOutput[3] = cellHeight * columnIndex; 
}

