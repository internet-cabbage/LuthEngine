#include "shader.h"
#include "stb_image.h"

#define currentDirectory 0
#define libFolder 1
#define textureFolder 2
#define shaderFolder 3
/* The dirType parameter determines which directory is found.
    -   dirType = 0: Top level project directory
    -   dirType = 1: lib folder
    -   dirType = 2: texture folder

*/
static char* findDirectory(int dirType) {
    static char buffer[PATH_MAX];
    uint32_t pathSize = (uint32_t) sizeof(buffer);
    _NSGetExecutablePath(buffer,&pathSize);
    //printf("Full path: %s \n", buffer);

    char *lastSlash = strrchr(buffer, '/');
    // I then 'cut' the string by setting the null pointer / end of the string to be where the slash is
    *(lastSlash+1) = '\0';
    if (dirType == currentDirectory) {
        return buffer;
    }
    if (dirType == libFolder) {
        strlcat(buffer,"libs/",sizeof(buffer));
        //printf("Lib directory: %s\n",buffer);
        return buffer;
    }
    if (dirType == textureFolder) {
        strlcat(buffer,"textures/",sizeof(buffer));
        //printf("Texture directory: %s\n",buffer);
        return buffer;
    }
    if (dirType == shaderFolder) {
        strlcat(buffer,"shaders/",sizeof(buffer));
        return buffer;
    }
    else {
        fprintf(stderr,"Unknown directory type specified in directory finder. \n");
        exit(-1);
    }
}

char* findTextureLocation(char* imageName) {
    static char textureLoc[PATH_MAX];
    char* dirLocation = findDirectory(textureFolder);
    strlcat(textureLoc,dirLocation,PATH_MAX);
    strlcat(textureLoc,imageName,PATH_MAX);
    printf("Texture location: %s \n\n",textureLoc);
    fflush(stdout);
    return textureLoc;
}

/*
void readTexture(char* imageName, unsigned int textArray[16], unsigned int index) {

    glGenTextures(1, &textArray[index]);
    glBindTexture(GL_TEXTURE_2D, textArray[index]);
    // Set texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    char filePath[PATH_MAX];
    char* filePathPtr = findDirectory();
    strcpy(filePath, filePathPtr);
    strcat(filePath, imageName);


    // Image properties
    int width, height, nrChannels;
    unsigned char *data = stbi_load(imageName, &width, &height, &nrChannels, 0);


   if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
   }
   else {
    fprintf(stderr, "Failed to load texture");
    exit(1);
   }
}
*/


static char* readShader(char* fileName) {
    FILE* fptr;
    char filePath[PATH_MAX];
    char* filePathPtr = findDirectory(shaderFolder);
    strcpy(filePath, filePathPtr);
    strcat(filePath, fileName);
    fptr = fopen(filePath,"r");
    if (fptr == NULL) {
        perror("An error occured (shader file pointer is NULL):\n");
        fprintf(stderr,"\nError location: shader.c");
        exit(1);
    }
    
    // Move to end of file
    fseek(fptr, 0, SEEK_END);
    // Returns current position in file
    size_t fileSize = (size_t) ftell(fptr); 
    // Move to beginning of file
    rewind(fptr);
    // Each character is stored using 1 byte
    char* shaderString = malloc(fileSize + 1); // extra byte allcated for the termnate '\0' character
    if (shaderString == NULL) {
        perror("Malloc failed:");
    }
    fread(shaderString,sizeof(char),fileSize,fptr);
    shaderString[fileSize] = '\0'; // Null terminator at end of string

    return shaderString;
} 

// A shader type of 'f' means fragment shader, and a type of 'v' means vertex shader.
static unsigned int createShader(char* shaderFileName, char shaderType) {
    int typeName; 
    if (shaderType == 'v') {
        typeName = GL_VERTEX_SHADER;
    }
    else if (shaderType == 'f') {
        typeName = GL_FRAGMENT_SHADER;
    }
    else {
        fprintf(stderr, "Shader type not supported. \n");
        exit(1);
    }
    
    unsigned int shader = glCreateShader(typeName);
    const char* shaderSource = readShader(shaderFileName);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    // Check if compilation was successful.

    int compileSuccess;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSuccess);

    if (!compileSuccess) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Error: Shader complilation failed. \n %s", infoLog);
    }
    return shader;
}


unsigned int linkShaders(char* vertexShaderName, char* fragmentShaderName) {
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    unsigned int vertexShader, fragmentShader;
    vertexShader = createShader(vertexShaderName, 'v');
    fragmentShader = createShader(fragmentShaderName, 'f');
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    glLinkProgram(shaderProgram);

    // Check if the linking was performed correctly
    int linkingSuccess;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkingSuccess);
    if (!linkingSuccess) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "ERROR: Shader linking failed. \n %s", infoLog);
    }
    // Delete already used shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

