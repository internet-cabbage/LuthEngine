#ifndef TREE_H
#define TREE_H

#include "types.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

extern node *pool;
extern size_t nodeCap;
extern size_t nodeCount;

node *poolAlloc(void);
void insertBody(node *targetNode, body *b, body* bodies, double antiSingularity);
vec3 calculateForce(node *tNode, body *b, double antiSingularity, double G, double Theta);


#endif