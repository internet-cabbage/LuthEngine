#include "tree.h"
#include "types.h"
#include "io.h"
#include "galaxy.h"
#include "sim.h"

#include <time.h>
#include <omp.h>

int main() {
    fprintf(stderr,"\nMax threads: %d \n", omp_get_max_threads());
    // Seeding the random generation function, so it doesnt repeat values
    srand(time(NULL));
    // Simulation parameters
    int count = 0; // Number of bodies
    int capacity = 9000; // Maximum number of bodies which the simulation can handle
    int tSteps = 200;
    double antiSingularity = 4.0;
    double G = 5;
    double theta = 1.0; 
    int xMax = 10000; // Maximum x distance
    int yMax = 10000; // Self explanatory
    int zMax = 10000;
    int vMax = 6; // Max velocity
    int mMax = 100; // Max mass
    int mMin = 90;
    double dt = 0.04;

    double *xVals = calloc(capacity, sizeof(double));
    double *yVals = calloc(capacity, sizeof(double));
    double *zVals = calloc(capacity, sizeof(double));
    double *vxVals = calloc(capacity, sizeof(double));
    double *vyVals = calloc(capacity, sizeof(double));
    double *vzVals = calloc(capacity, sizeof(double));
    double *mVals = randomGen(mMin,mMax, capacity);

    // Parameters for galaxy spawning
    vec3 xOff1 = {0.0, 0.0, 0.0}; vec3 vOff1 = {0.0, 0.0, 0.0};
    vec3 xOff2 = {7000.0, 0.0, 0.0}; vec3 vOff2 = {-1.0, 0.0, 0.0};

    // plummerFunc(int *capacity, int NSpawn, int count, double* xVals, double* yVals, double* zVals, double* vxVals, double* vyVals, double* vzVals, vec3 xOff, vec3 vOff, double aConst, double G, double* MVals) {

    double aConst1 = 1000.0;
    count += plummerFunc(&capacity,6000,count,xVals,yVals,zVals,vxVals,vyVals,vzVals,xOff1,vOff1,aConst1,G,mVals, 6000);
    count += distributionFunction(&capacity, 3000, count, 3000,-0.0015, 0, xVals, yVals, zVals, vxVals, vyVals, vzVals, xOff2, vOff2);

    //double *vxVals = randomContinuous(vMax, N);
    //double *vyVals = randomContinuous(vMax, N);
    

    // Variable for ensuring the simulation doesnt instantly crash my computer.
    int framesPerWrite = 1;

    // Initialise bodies loop

    body *bodies = calloc(count, sizeof(body));
    for (int i = 0; i < count; i++) {
        vec3 position, velocity;
        position.x = xVals[i];
        position.y = yVals[i];
        position.z = zVals[i];

        velocity.x = vxVals[i];
        velocity.y = vyVals[i];
        velocity.z = vzVals[i];

        bodies[i].position = position;
        bodies[i].velocity = velocity;
        bodies[i].mass = mVals[i]; 
        bodies[i].id = i;
    }

    free(xVals);
    free(yVals);
    free(zVals);
    free(vxVals);
    free(vyVals);
    free(vzVals);
    free(mVals);
    xVals = yVals = zVals = vxVals = vyVals = vzVals = mVals = NULL;

    /*
    In order for my chosen integrator to work, I need to calculate the initial acceleration, so I can 'offset' the velocity
    by half of a time step. This integrator method is known as the leapfrog integrator, and it is better at conserving energy than
    the euler step method I was originally going to use.    
    */ 

    // Using the pool memory allocator, for cache locality

    // nodeCap determines how many nodes can be stored in the memory pool, any more nodes than this will cause the pool to overflow and the program to presumably become very corrupted

    nodeCap = 32 * count;
    pool = calloc(nodeCap, sizeof(node));
    nodeCount = 0;

    if (pool == NULL) {
        printf("Pool memory allocation failed!!! PANIC!!! \n");
        exit(1);
    }

    fprintf(stderr, "Initial tree build starting. \n");
    // Creating the root node
    node* rootPtr = poolAlloc();
    *rootPtr = (node){0};
    rootPtr->xmax = xMax * 2;
    rootPtr->xmin = -xMax * 2;
    rootPtr->ymax = yMax * 2;
    rootPtr->ymin = -yMax * 2;
    rootPtr->zmax = zMax * 2;
    rootPtr->zmin = -zMax * 2;
    // Creating the tree

    // Inserts all bodies into the tree to populate it
    for (int i = 0; i < count; i++) {
        insertBody(rootPtr, &bodies[i], bodies, antiSingularity);
    }
    // Uses the lovely tree to calculate the formaces and accelerations on all the bodies
    vec3 *accelArray = calloc(count, sizeof(vec3));
    for (int i = 0; i < count; i++) {
        vec3 f = calculateForce(rootPtr, &bodies[i], antiSingularity, G, theta);
        accelArray[i].x = f.x / bodies[i].mass;
        accelArray[i].y = f.y / bodies[i].mass;
        accelArray[i].z = f.z / bodies[i].mass;
    }

    // Offsets the velocity to apply the leapfrog correction

    for (int i = 0; i < count; i++) {
        bodies[i].velocity.x -= accelArray[i].x * (dt/2); 
        bodies[i].velocity.y -= accelArray[i].y * (dt/2); 
        bodies[i].velocity.z -= accelArray[i].z * (dt/2); 
    }
    fprintf(stderr, "Initial tree build successful. \n");
    fprintf(stderr, "Node count after initial build: [%d / %d] \n", nodeCount, nodeCap);
    
    // Now I free all the memory used to perform those calculations

    //freeTree(rootPtr);
    free(accelArray);

    fprintf(stderr, "Initial tree collapse successful. \n");

    // This is the main loop which performs all the calculations, and saving the data

    // File to write to
    FILE *dataFile = fopen("DataOutput.bin", "wb");

    // Checking if the file was actually created, as who knows what the C program would do otherwise
    if (dataFile == NULL) {
        fprintf(stderr ,"Output file can't be created:");
    }
    else {
        fprintf(stderr, "Output file successfully created. \n \nBeginning calculations: \n \n");
    }

    // Writes the parameters required for the PythonRenderer to interpret the data

    int writesteps = tSteps / framesPerWrite;
    fwrite(&count,sizeof(count),1,dataFile);
    fwrite(&writesteps,sizeof(tSteps),1,dataFile);
    //fwrite(&antiSingularity, sizeof(antiSingularity),1,dataFile);

    // The array storing the positions of all the bodies, to be written to a buffer
    float * frameBuffer = calloc(3*count, sizeof(float));

    // Initial conditions of the system
    writeFrame(dataFile,bodies,count,frameBuffer);

    // Code for the loading bar
    static char barString[] = "||||||||||||||||||||||||||||||||||||||||";
    static int barLength = 40;

    int barInterval = (tSteps / (11 * barLength));

    for (int j = 0; j < tSteps; j++) {
        if ((j) % barInterval == 0) {
            printProgress(j, tSteps, barLength);
        }
        timeLoop(bodies,count,antiSingularity,G,tSteps,dt,theta,xMax,yMax,zMax);
         /* And finally writing this output to a file, each row will contain the state for a single body at a single time
        [xPos_0, yPos_0, xPos_1, yPos_1...]
        */
       if (j % framesPerWrite == 0) {
            writeFrame(dataFile,bodies,count,frameBuffer);
       }
    }
    // Completes the print bar so that it is always at 100% at the end of the program.
    printProgress(tSteps,tSteps, barLength);
    fclose(dataFile);
    free(frameBuffer);
    free(bodies);
    free(pool);

    fprintf(stderr, "\nBuild time: %.2f, force time: %.2f \n \n", buildTime, forceTime);
}