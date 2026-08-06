#include "dataImporter.h"

/* 
This function accepts a binary file as input data, which has to be formatted such that the first 8 bytes 
(or whatever the size of 2 int values is on the compiler) contain two integers:
    - First value: Number of stars N
    - Second value: Number of time steps / frames to render.
*/
int N = -1;
int tSteps = -1;
RGB* colourVals;


void importPreChecks(char* filePath) {
    // Opens the file and stores a pointer to where it is stored
    FILE* fptr = fopen(filePath, "rb");
    if (fptr == NULL) {
        perror("ERROR: Unable to retrieve star position data from binary:\n");
        fprintf(stderr, "\n Error location: dataImporter.c\n");
        exit(-1);
    }

    // Reads the parameter info
    fread(&N,sizeof(int),1,fptr); // formatting data
    fread(&tSteps,sizeof(int),1,fptr);
    colourVals = calloc(N,sizeof(RGB));
    fread(colourVals,sizeof(RGB),N,fptr);
    
    // Moves to the end of the file, and calculates the file size
    int status = fseek(fptr,0,SEEK_END);
    if (status != 0) {perror("File end unable to be located\n");} // Basic error checking

    fprintf(stdout,"Star parameters: (N: %d, tSteps: %d)\n", N, tSteps);
    
    long fileSize = ftell(fptr);
    long expectedFileSize = (2L * sizeof(int)) + (N * sizeof(RGB)) + (3L * N * tSteps * sizeof(float));

    if (fileSize != expectedFileSize) {
        fprintf(stderr, "ERROR: Actual file size differs from expected value\n");
        printf("Actual file size: %ld\n Expected file size: %ld\n", fileSize, expectedFileSize);
        fflush(stderr);
        fflush(stdout);
        exit(-1);
    }
    fflush(stdout);
    fclose(fptr);
}

vec3* importStarFrame(char* filePath, int frameIndex) {
    if (frameIndex < 0 || frameIndex > tSteps) {
        fprintf(stderr, "Frame index outside of saved frame data\n");
        fflush(stderr);
    }
    // Opens the file and stores a pointer to where it is stored
    FILE* fptr = fopen(filePath, "rb");
    if (fptr == NULL) {
        perror("ERROR: Unable to retrieve star position data from binary:\n");
        fprintf(stderr, "\n Error location: dataImporter.c\n");
        exit(-1);
    }

    vec3* starData = calloc((size_t) N, sizeof(vec3));
    if (starData == NULL) {
        perror("ERROR: Unable to reserve enough memory to import star data.\n");
        exit(-1);
    }

    size_t startIndex = (2 * sizeof(int)) + (N * sizeof(RGB)) + (3L * frameIndex * N * sizeof(float));
    fseek(fptr,startIndex,SEEK_SET);
    fread(starData,sizeof(vec3),(size_t) N, fptr);
    
    fclose(fptr);

    return starData;
}