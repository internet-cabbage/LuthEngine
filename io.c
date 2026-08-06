#include "io.h"


// Cute little helper function to make it easier to write data to the binary file
void writeFrame(FILE *dataFile, body *bodies, size_t N, float *frameBuffer) {
    for (int i = 0; i < (int) N; i++) {
        frameBuffer[3*i] = (float)bodies[i].position.x;
        frameBuffer[(3*i)+1] = (float)bodies[i].position.y;
        frameBuffer[(3*i)+2] = (float)bodies[i].position.z;
    }
    fwrite(frameBuffer, sizeof(float), 3*N, dataFile);
}

void printProgress(int stepVal, int tSteps, int width) {
    double perVal = (double) stepVal / (tSteps) * 100;

    char progBar[] = "========================================================================================================================";
    char emptyBar[] = "------------------------------------------------------------------------------------------------------------------------";

    // Width of the filled bar
    int filledWidth = (int) (perVal / 100 * width);
    int emptyPad = width - filledWidth;
    printf("%.s", emptyBar);
    // Bunch of mumbo jumbo I took ages to figure out
    // First bit (%5.2f%) just means 'print this number to 2 decimal places, and make it take up at least 5 places'
    // %.*s Basically just means 'Input how many characters to print of a string, input the string to be truncated'
    printf("\r %5.2f%% [%.*s%.*s] tStep: %5.d / %d", perVal, filledWidth, progBar, emptyPad, emptyBar, stepVal, tSteps);
    fflush(stdout);
}