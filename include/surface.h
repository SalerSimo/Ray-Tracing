#ifndef SURFACE_H
#define SURFACE_H

#include<stdint.h>
#include"geometry.h"
#include"color.h"

#define LAT_DIVS 20
#define LON_DIVS 20
#define MAX_DEPTH 3

typedef struct{
    Point *a, *b, *c;
}Triangle;

typedef enum{
    SPHERE, GENERIC
}SurfaceType;

typedef struct{
    Triangle **triangles;
    int numTriangles;
    Point *center;
    double maxDistanceFromCenter;
    Color color;
    SurfaceType type;
    double reflexivity;
    double smoothness;
}Surface;

// Constructor
/**
 * Creates a rect parallel to X-Y plane.
 */
Surface *Surface_createRectXY(Point *origin, double width, double height, double reflexivity, double smoothness, Color color);
/**
 * Creates a rect parallel to X-Z plane.
 */
Surface *Surface_createRectXZ(Point *origin, double width, double height, double reflexivity, double smoothness, Color color);

/**
 * Creates a 3D box-shaped Surface object composed of 12 triangles (2 per face).
 * 
 * The box is defined by an origin point (lowest x, y, z corner), width, height,
 * and depth. The function computes the eight corner points of the box,
 * constructs triangles for each face, and initializes the Surface properties
 * such as color, reflexivity, and opacity.
 * 
 * Parameters:
 *  - `origin`: Pointer to the Point representing the minimum corner of the box.
 *  - `width`: Length of the box along the x-axis.
 *  - `height`: Length of the box along the y-axis.
 *  - `depth`: Length of the box along the z-axis.
 *  - `reflexivity`: Surface reflection coefficient (0 = no reflection, 1 = perfect mirror).
 *  - `smoothness`: Surface smoothness (0 = fully rough, 1 = fully smooth).
 *  - `color`: 32-bit RGB color of the box surface.
 * 
 * Returns:
 *  - Pointer to the allocated Surface representing the box.
 *  - NULL if allocation fails.
 */
Surface *Surface_createBox(Point *origin, double width, double height, double depth, double reflexivity, double smoothness, Color color);
Surface *Surface_createSphere(Point *center, double radius, double reflexivity, double smoothness, Color color);

Vector Triangle_getNormal(Triangle *t);

#endif //SURFACE_H