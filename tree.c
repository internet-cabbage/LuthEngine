#include <math.h>
#include <stdlib.h>

#include "tree.h"


// ================================================================
// Memory management function
// ================================================================

/*
Instead of storing all the node data in different places in cache, I can allocate one large pool of memory and store them all contiguously.
*/
node *pool; // A pointer to where all the nodes will be stored
int nodeCap; // The max amount of nodes in the pool
int nodeCount; // How many nodes are currently in the pool

// A bump allocator used to simplify the process of freeing all the memory
node* poolAlloc() {
    if (nodeCount >= nodeCap-1) {
        printf("\n \n Pool overflow error at: [%d/%d] \n", nodeCount, nodeCap);
        exit(1);
    }
    // The adress at which the new node is to be stored
    node* nodePtr = &pool[nodeCount];
    nodeCount++;
    *nodePtr = (node){0}; // Clears the node of previous data

    // Returns a pointer to where the node should be stored
    return nodePtr;
}


// ================================================================
// Quadtree helper functions
// ================================================================

// This function returns 1 if the body is within the node, and 0 otherwise
static inline int isBodyInside(node *tNode, body *b) {
    // tNode just means targetNode, but it takes up less space on my screen
    if ((tNode->xmin < b->position.x) && (b->position.x < tNode->xmax) && (tNode->ymin < b->position.y) && (b->position.y < tNode->ymax) && (tNode->zmin < b->position.z) && (b->position.z < tNode->zmax)) {
        //printf("Node x-range (%f <-> %f), y-range (%f <-> %f) \nBody Coords (%f, %f) \nBody id: (ID %d) \n \n", tNode->xmin, tNode->xmax, tNode->ymin, tNode->ymax, b->position.x, b->position.y, b->id);
        return 1;
    }
    else {
        return 0;
    }
}

// This returns the length of the sides of the nodes, assuming that the node is a square

static inline double length(node *tNode) {
    return (tNode->xmax - tNode->xmin);
}

static int compareInside(int searchId, node insertNode) {
    for (int i = 0; i < insertNode.count; i++) {
        if (searchId == insertNode.childIDs[i]) {
            return 1;
        }
    }
    
    return 0;
}

static void octDivide(node *tNode) {
    double xmid = (tNode->xmin + tNode->xmax) / 2;
    double ymid = (tNode->ymin + tNode->ymax) / 2;
    double zmid = (tNode->zmin + tNode->zmax) / 2;

    /* top of cube
    [0,1]
    [2,3] 
    */

    /* bottom of cube
    [4,5]
    [6,7]
    */

    node emptyNode;

    tNode->children[0] = poolAlloc();
    *tNode->children[0] = (node){.xmin = tNode->xmin, .xmax = xmid, .ymin = ymid, .ymax = tNode->ymax, .zmin = zmid, .zmax = tNode->zmax, .count = 0};

    tNode->children[1] = poolAlloc();
    *tNode->children[1] = (node){.xmin = xmid, .xmax = tNode->xmax, .ymin = ymid, .ymax = tNode->ymax, .zmin = zmid, .zmax = tNode->zmax, .count = 0};
    
    tNode->children[2] = poolAlloc();
    *tNode->children[2] = (node){.xmin = tNode->xmin, .xmax = xmid, .ymin = tNode->ymin, .ymax = ymid, .zmin = zmid, .zmax = tNode->zmax, .count = 0};
    
    tNode->children[3] = poolAlloc();
    *tNode->children[3] = (node){.xmin = xmid, .xmax = tNode->xmax, .ymin = tNode->ymin, .ymax = ymid, .zmin = zmid, .zmax = tNode->zmax, .count = 0};

    tNode->children[4] = poolAlloc();
    *tNode->children[4] = (node){.xmin = tNode->xmin, .xmax = xmid, .ymin = ymid, .ymax = tNode->ymax, .zmin = tNode->zmin, .zmax = zmid, .count = 0};

    tNode->children[5] = poolAlloc();
    *tNode->children[5] = (node){.xmin = xmid, .xmax = tNode->xmax, .ymin = ymid, .ymax = tNode->ymax, .zmin = tNode->zmin, .zmax = zmid, .count = 0};
    
    tNode->children[6] = poolAlloc();
    *tNode->children[6] = (node){.xmin = tNode->xmin, .xmax = xmid, .ymin = tNode->ymin, .ymax = ymid, .zmin = tNode->zmin, .zmax = zmid, .count = 0};
    
    tNode->children[7] = poolAlloc();
    *tNode->children[7] = (node){.xmin = xmid, .xmax = tNode->xmax, .ymin = tNode->ymin, .ymax = ymid, .zmin = tNode->zmin, .zmax = zmid, .count = 0};

}


// This little function serves to help the insertBody function, by calculating
// which index position to insert the new node at
static inline int insertIndex(node *parentNode, double x, double y, double z) {
    double xmid = (parentNode->xmin + parentNode->xmax) / 2.0;
    double ymid = (parentNode->ymin + parentNode->ymax) / 2.0;
    double zmid = (parentNode->zmin + parentNode->zmax) / 2.0;


    // top of cube
    if (zmid < z) {
        // Top nodes
        if (ymid < y) {
            // Is node on the right?
            if (xmid < x) {
                return 1;
            }
            // top left node
            else {
                return 0;
            }
        }
        // bottom nodes
        else {
            // bottom right node
            if (xmid < x) {
                return 3;
            }
            // bottom left node
            else {
                return 2;
            }
        }
    }
    // bottom of cube
    else {
        if (ymid < y) {
            if (xmid < x) {
                return 5;
            }
            else {
                return 4;
            }
        }
        else {
            if (xmid < x) {
                return 7;
            }
            else {
                return 6;
            }
        }

    }

}



void insertBody(node *targetNode, body *b, body* bodies, double antiSingularity) {

    /* The process for inserting goes as follows:
        1) Check if the body being inserted, is even within the node
        2) if so, check if the node has children
            3.a) if yes, then recursively insert this body into a child node
            3.b) if no, then check if this node is occupied:
                4.a) If the node is occupied (i.e a leaf node), give this node 4 kids, and insert the old node
                into one of the child nodes, and do the same for the node being inserted. If they both get inserted into the same node
                then recursively repeat this process until they are at different locations
                4.b) If the node isnt occupied, set the node info to that of the body
    */

    // If it returns one, then the body IS in the node

    if (isBodyInside(targetNode, b) == 1) {
        // returns one if the targetNode does have kids
        // If it has kids, then we update this node, and insert the body into one of its kids
        if (targetNode->children[0] != NULL) {

            // Firstly we have to update the center of mass and total mass of the parent node

            double newTotalMass = targetNode->massTot + b->mass;
            
            // This is just the standard centre of mass formula from physics
            targetNode->cx = ((targetNode->cx * targetNode->massTot)+(b->position.x * b->mass)) / newTotalMass;
            targetNode->cy = ((targetNode->cy * targetNode->massTot)+(b->position.y * b->mass)) / newTotalMass;
            targetNode->cz = ((targetNode->cz * targetNode->massTot)+(b->position.z * b->mass)) / newTotalMass;
            targetNode->massTot = newTotalMass;
            
            // Finds the index position of the node the body should be inserted in
            int index = insertIndex(targetNode, b->position.x, b->position.y, b->position.z);
            // Recursively calls the function until it is inserted
            insertBody(targetNode->children[index],b, bodies, antiSingularity);
        }
        else {
            // Check if the node is a leaf node
            if (targetNode->massTot != 0.0 && length(targetNode) > 2 * antiSingularity) {
                // Finds the location to put the 'native' node and new node into
                body nodeBody = bodies[targetNode->childIDs[0]];
                int nativeIndex = insertIndex(targetNode, nodeBody.position.x, nodeBody.position.y, nodeBody.position.z);
                int newIndex = insertIndex(targetNode, b->position.x, b->position.y, b->position.z);

                // Creates 4 child nodes
                octDivide(targetNode);

                // The native node gets inserted into the correct child, then removed from the native node
                insertBody(targetNode->children[nativeIndex],&nodeBody, bodies, antiSingularity);
                targetNode->childIDs[0] = -1;
                targetNode->count = 0;

                // Inserts the new node into the correct place
                insertBody(targetNode->children[newIndex], b, bodies, antiSingularity);

                // Now we have to recompute the centre of mass and its location
                double newTotalMass = targetNode->massTot + b->mass;
            
                targetNode->cx = ((targetNode->cx * targetNode->massTot)+(b->position.x * b->mass)) / newTotalMass;
                targetNode->cy = ((targetNode->cy * targetNode->massTot)+(b->position.y * b->mass)) / newTotalMass;
                targetNode->cz = ((targetNode->cz * targetNode->massTot)+(b->position.z * b->mass)) / newTotalMass;
                targetNode->massTot = newTotalMass;
                
            }
            else if (length(targetNode) <= 2 * antiSingularity) {
                if (targetNode->count >= 15) {
                    printf("Number of child nodes in node exceeded 15, nodeLength: %f \n", length(targetNode));
                    exit(1);
                }
                targetNode->childIDs[targetNode->count] = b->id;
                targetNode->count++;

                double newTotalMass = targetNode->massTot + b->mass;
            
                targetNode->cx = ((targetNode->cx * targetNode->massTot)+(b->position.x * b->mass)) / newTotalMass;
                targetNode->cy = ((targetNode->cy * targetNode->massTot)+(b->position.y * b->mass)) / newTotalMass;
                targetNode->cz = ((targetNode->cz * targetNode->massTot)+(b->position.z * b->mass)) / newTotalMass;
                targetNode->massTot = newTotalMass;

            }
            else {
                // If the targetNode does not have kids, then it is a leaf node, so we insert the body here
                // This updates the value of the 'nodeBody' attribute of the targetNode struct, to be equal to the value held by pointer b
                targetNode->childIDs[0] = b->id;
                targetNode->count++;
                targetNode->massTot = b->mass;
                targetNode->cx = b->position.x;
                targetNode->cy = b->position.y;
                targetNode->cz = b->position.z;
            }
        }
    }
}


// ================================================================
// Force calculator function
// ================================================================

// This calculates the force that a specific node 'tNode' is acting on the body 'b'
vec3 calculateForce(node *tNode, body *b, double antiSingularity, double G, double Theta) {

    // If its total mass is zero, it contains no bodies and thus exerts no force
    if (tNode->massTot == 0.0) {
        return (vec3) {0.0,0.0,0.0};
    }
    /* if the node being considered is the one containing the body, then it doesnt exactly exert any force on itself does it?
     The body is the same as the one within the node if they have the same pointer 
     you cant directly compare structs in c, so you gotta do this for that reason as well

     THE ABOVE IS A LIE!!! I am leaving it there incase I make the same error. But the node only stores a copy of the object, not its pointer
     so the body in the node and the real body will have different adresses
    */
    else if (compareInside(b->id, *tNode) == 1) {
        return (vec3) {0.0,0.0,0.0};
    }

    // Calculates the vector distance between the center of mass of the node, and the body position
    else {
        double dx = tNode->cx - b->position.x;
        double dy = tNode->cy - b->position.y;
        double dz = tNode->cz - b->position.z;
        double r2 = (dx * dx) + (dy * dy) + (dz * dz) + (antiSingularity * antiSingularity);
        

        double nodeSize = length(tNode);

        // This decides whether or not to use the approximation.
        // If the node has no child nodes, applying the approximation is the same as directly summing it
        // Instead of the standard nodeSize/r < theta comparison, I squared it so that I dont have to perform an unnecesary square root
        if (tNode->children[0] == NULL || ((nodeSize*nodeSize)/r2) < Theta*Theta) {
            
            double fMag = (G * tNode->massTot * b->mass) / (r2);
            double r = sqrt(r2);
            double fx = fMag * (dx/r);
            double fy = fMag * (dy/r);
            double fz = fMag * (dz/r);
            return (vec3) {fx,fy,fz};
        }
        // If the nodes are too close to approximate, it recursively calculates it directly
        
        else {
            vec3 totalForce = {0.0,0.0,0.0};
            for (int i = 0; i < 8; i++) {
                if (tNode->children[i] != NULL) {
                    vec3 recursiveForce = calculateForce(tNode->children[i], b, antiSingularity, G, Theta);
                    totalForce.x += recursiveForce.x;
                    totalForce.y += recursiveForce.y;
                    totalForce.z += recursiveForce.z;
                }
            }
            return totalForce;
        }
    }
}