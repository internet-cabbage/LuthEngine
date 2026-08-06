#include "galaxy.h"

double piVal = 3.14159265;

// ================================================================
// Random generation functions
// ================================================================


double* randomGen(int lower, int upper, size_t N) {
    // Adress at which the array is saved at
    double* adress = calloc(N, sizeof(double));

    int intN = (int) N;
    for (int i = 0; i < intN; i++) {
        double val = (rand() % (upper-lower+1)) + lower;
        *(adress + i) = val;
    }
    return adress;
}

double* randomContinuousPositive(double maxMag, size_t N) {
    double* adress = calloc(N, sizeof(double));

    int intN = (int) N;
    for (int i = 0; i < intN; i++) {
        double val = maxMag * (double) rand()/RAND_MAX;
        *(adress + i) = val;
    }
    return adress;
}

double* randomContinuous(double maxMag, size_t N) {
    double *adress = calloc(N, sizeof(double));

    int intN = (int) N;
    for (int i = 0; i < intN; i++) {
        if ((double)rand()/RAND_MAX > 0.5) {
            double val = maxMag * (double) rand()/RAND_MAX;
            *(adress + i) = val;
        }
        else {
            double val = - maxMag * (double) rand()/RAND_MAX;
            *(adress + i) = val;
        }
    }
    return adress;
}

// Generates an array of random probabilities in the range [0,1]
double* randUSample(size_t N) {
    double* adress = calloc(N, sizeof(double));
    int intN = (int) N;
    for (int i = 0; i < intN; i++) {
        adress[i] = (double) rand()/RAND_MAX;
    }
    return adress;
}

// ================================================================
// Actual distribution function(s)
// ================================================================

double gFunc(double q) {
    double g = q * q * pow(1 - (q*q), 3.5);
    return g;
}

size_t plummerFunc(size_t *capacity, size_t NSpawn, size_t count, double* xVals, double* yVals, double* zVals, double* vxVals, double* vyVals, double* vzVals, vec3 xOff, vec3 vOff, double aConst, double G, double* MVals, double maxR, RGB* colourVals) {

    if (count + NSpawn > *capacity) {
        printf("Too many bodies spawned");
        exit(0);
    }
    
    // Give all the pieces of a dust a grey colour

    for (int i = (int) count; i < (int)(count + NSpawn); i++) {
        colourVals[i] = (RGB){199,210,212};
    }

    double totalMass = 0.0;

    for (int i = (int) count; i < (int) (count + NSpawn); i++) {
        totalMass += MVals[i];
    }
    
    // Randomly generates a number between zero and 1
    double* u = randUSample(NSpawn);
    double* temp1 = randUSample(NSpawn);
    double* temp2 = randUSample(NSpawn);
    double* temp3 = randUSample(NSpawn);
    double* temp4 = randUSample(NSpawn);

    double* azimuthalAngles = calloc(NSpawn,sizeof(double));
    double* polarAngles = calloc(NSpawn,sizeof(double));

    double* azimuthalAnglesVel = calloc(NSpawn,sizeof(double));
    double* polarAnglesVel = calloc(NSpawn,sizeof(double));
    
    // Formula from:
    // https://mathworld.wolfram.com/SpherePointPicking.html
    for (int i = 0; i < (int) NSpawn; i++) {
        azimuthalAngles[i] = 2.0 * piVal * temp1[i];
        polarAngles[i] = acos(2.0 * temp2[i] - 1);

        azimuthalAnglesVel[i] = 2.0 * piVal * temp3[i];
        polarAnglesVel[i] = acos(2.0 * temp4[i] - 1);
    }
    
    //double* azimuthalAngles = randomContinuousPositive(2 * pi, *capacity);
    //double* polarAngles = randomContinuousPositive(pi, *capacity);

    // In order to calculate the velocity of a star, I need to calculate its escape velocity

    double* escapeVelocity = calloc(NSpawn, sizeof(double));

    double* radialDistances = calloc(NSpawn, sizeof(double));
    double g_max = 0.0922;
    double vScalar = 0.0;
    for (int i = (int) count; i < (int) (NSpawn + count); i++) {
        int j = i - (int)count;
        double crtU = pow(u[j], 1.0/3.0);
        int insideRadius = 0;
        while (insideRadius == 0) {
            radialDistances[j] = (aConst * crtU) / pow(1-(crtU * crtU), 0.5);
            if (radialDistances[j] < maxR) {insideRadius = 1;}
            else {
                u[j] = (double) rand()/RAND_MAX;
                crtU = pow(u[j], 1.0/3.0);
            } }
        
            xVals[i] = radialDistances[j] * sin(polarAngles[j]) * cos(azimuthalAngles[j]) + xOff.x;
            yVals[i] = radialDistances[j] * sin(polarAngles[j]) * sin(azimuthalAngles[j]) + xOff.y;
            zVals[i] = radialDistances[j] * cos(polarAngles[j]) + xOff.z;


            double vNumerator = pow(2*G*totalMass,0.5);
            double vDenominator = pow(radialDistances[j]*radialDistances[j] + aConst*aConst,0.25);
            escapeVelocity[j] = vNumerator/vDenominator;
            int velSelected = 0;
            
            while (velSelected == 0) {
                double q = (double) rand()/RAND_MAX;
                double x2 = (double) rand()/RAND_MAX;
                if (g_max * x2 < gFunc(q)) {
                    vScalar = q * escapeVelocity[j];
                    velSelected = 1;
                }
            vxVals[i] = vScalar * sin(polarAnglesVel[j]) * cos(azimuthalAnglesVel[j]) + vOff.x;
            vyVals[i] = vScalar * sin(polarAnglesVel[j]) * sin(azimuthalAnglesVel[j]) + vOff.y;
            vzVals[i] = vScalar * cos(polarAnglesVel[j]) + vOff.z;
        }   
    }

        

    free(polarAngles);
    free(azimuthalAngles);
    free(temp1); free(temp2); free(temp3); free(temp4);
    free(polarAnglesVel);
    free(azimuthalAnglesVel);

    free(u);
    free(escapeVelocity);
    free(radialDistances);

    return NSpawn;
}

size_t distributionFunction(size_t *capacity, size_t NSpawn, size_t count, double maxR, double rotSpeed, int genType, double* xVals, double* yVals, double* zVals, double* vxVals, double* vyVals, double* vzVals, vec3 xOff, vec3 vOff, RGB* colourVals) {
    if (genType == 0) {

        // Colour values
        for (int i = (int)count; i < (int)(count + NSpawn); i ++) {
            colourVals[i] = (RGB) {237,180,132};
        }
        double *randThetas = randomContinuousPositive(2 * piVal, *capacity);
        double *randZWobble = randomContinuous(maxR*0.05, *capacity);
        /* 
        Well the problem is that the random number generator I am using, is a continuous distribution.
        And I need the radial distance distribution to not be continuous, as the stellar distance density
        has to be directly proportional to the radial distance.
        */
       double *squaredRadialDistances = randomContinuous(maxR * maxR, *capacity);
       double *radialDistances = calloc(*capacity, sizeof(double));

       for (int i = (int) count; i < (int) (NSpawn + count); i++) {
            // Handle the square root of a negative number
            if (squaredRadialDistances[i] > 0) {
                radialDistances[i] = sqrt(squaredRadialDistances[i]);
            }
            else {
                radialDistances[i] = -sqrt(-squaredRadialDistances[i]);
            }
            xVals[i] = (radialDistances[i] * cos(randThetas[i])) + xOff.x;
            yVals[i] = (radialDistances[i] * sin(randThetas[i])) + xOff.y;
            zVals[i] = (randZWobble[i]) + xOff.z;

            double v = rotSpeed * radialDistances[i];
            vxVals[i] = (v * sin(randThetas[i])) + vOff.x;
            vyVals[i] = (-v * cos(randThetas[i])) + vOff.y;
            vzVals[i] = vOff.z;
            colourVals[i] = (RGB) {250, 180, 120};
       }

       // Close distance

       double minDist = 1e30;
       int body1 = -1, body2 = -1;

       for (int i = (int) count; i < (int) (NSpawn + count); i++) {
            for (int j = i+1; j < (int) NSpawn; j++) {
                double dx = xVals[i] - xVals[j], dy = yVals[i] - yVals[j], dz = zVals[i] - zVals[j];
                double dist = sqrt(dx*dx + dy*dy + dz*dz);
                if (dist < minDist) {
                    minDist = dist; body1 = i; body2 = j;
                }
            }
       }
       printf("min separation %.3e between body %d and %d \n", minDist, body1, body2);
    }
    else {
        printf("Chosen generator type does not exist.");
    }
    return NSpawn;
}
