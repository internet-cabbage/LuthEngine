
#include <math.h>
#include <omp.h>
#include "sim.h"
#include "tree.h"

double buildTime = 0.0;
double forceTime = 0.0;

void timeLoop(body *bodies, size_t N, double antiSingularity, double G, double dt, double theta, int xmax, int ymax, int zmax) {
    // The array storing the acceleration info for the bodies
    int intN = (int) N;
    vec3 *accelArray = calloc(N, sizeof(vec3));

    /*
    When outputed to the binary file, I need to specify the value of N,
    as it determines how large each frame is in the file. So I just output the value N
    as the first item in the file.
    */

    // So firstly I have to create the entire tree datastructure, by recursively inserting each node

    nodeCount = 0;
    node* rootPtr = poolAlloc();
    *rootPtr = (node){0};
    rootPtr->xmax = xmax * 2; rootPtr->xmin = -xmax * 2;
    rootPtr->ymax = ymax * 2; rootPtr->ymin = -ymax * 2;
    rootPtr->zmax = zmax * 2; rootPtr->zmin = -zmax * 2;

    double t0 = omp_get_wtime();
    // Inserts all bodies into the tree to populate it
    for (int i = 0; i < intN; i++) {
        insertBody(rootPtr, &bodies[i], bodies, antiSingularity);
    }
    
    double t1 = omp_get_wtime();
    // Uses the lovely tree to calculate the forces and accelerations on all the bodies
    // It also makes use of openmp to paralellise it
    #pragma omp parallel for schedule(dynamic, 32)
    for (int i = 0; i < intN; i++) {
        vec3 a = calculateForce(rootPtr, &bodies[i], antiSingularity, G, theta);
        
        bodies[i].velocity.x += a.x * dt;
        bodies[i].position.x += bodies[i].velocity.x * dt;


        bodies[i].velocity.y += a.y * dt;
        bodies[i].position.y += bodies[i].velocity.y * dt;
                
        bodies[i].velocity.z += a.z * dt;
        bodies[i].position.z += bodies[i].velocity.z * dt;
    }
    double t2 = omp_get_wtime();


    buildTime += t1 - t0;
    forceTime += t2 - t1;

    free(accelArray);
    
}