#ifndef TYPES_H
#define TYPES_H


typedef struct {
    double x,y,z;
} vec3;

typedef struct {
    vec3 position, velocity;
    double mass;
    // Stores a unique identifier for each body, to avoid self comparison
    int id;
    // 0 for star, 1 for dark matter, 2 for gas
    int type;
} body;

typedef struct node {
    // Coordinates of the bounding box of the node
    double xmin, xmax, ymin, ymax, zmin, zmax;
    // Total mass of the node
    double massTot;
    // Position of the center of mass
    double cx,cy,cz;
    // If the node is a leaf node, this variable stores the body data
    int childIDs[16];

    int count;
    // the children in the node [top left, top right, bottom left, bottom right]
    /*
    [0,1]
    [2,3] 
    */
    struct node* children[8];
} node;


#endif