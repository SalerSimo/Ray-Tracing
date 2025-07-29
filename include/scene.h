#ifndef SCENE_H
#define SCENE_H

#include<stdio.h>
#include<stdlib.h>

#include"geometry.h"
#include"surface.h"

typedef struct{
    Point *position;
    double radius;
    Color color;
}Light;

/**
 * Represents a 3D scene for ray tracing.
 *
 * Contains camera position, light source, and an array of surfaces that define the geometry.
 * It also includes camera-related properties like the viewing distance and rotation angle.
 */
typedef struct{
    /** Pointer to the camera position. */
    Point *camera;
    /** Distance between the camera and the view plane.*/
    double cameraDistance;
    /** 
     *  Rotational angle in radians around the Y-axis, applied to the camera.
     *  An angle of 0 means the camera is facing the negative Z-axis.
     *  A positive angle corresponds to anti-clockwise rotation.
     */
    double rotationAngle;
    /** Array of pointers to the surfaces in the scene. */
    Surface **surfaces;
    /** Number of surfaces in the scene. */
    unsigned int numSurfaces;
    /** Pointer to the light source of the scene. */
    Light *lightSource;
}Scene;


/**
 * Creates an empty scene.
 * 
 * It sets all pointers to NULL and all numeric values to 0.
 * @return Pointer to the allocated scene or NULL if allocation fails.
 */
Scene *Scene_init();

/**
 * Initializes a scene with the given light source and surfaces.
 * 
 * @param s The scene to fill.
 * @param lightSource Pointer to the position of the light source.
 * @param surfaces Array of pointers to the surfaces to include in the scene.
 * @param numSurfaces Number of surfaces in the array.
 */
void Scene_fill(Scene *s, Light *lightSource, Surface **surfaces, int numSurfaces);


Light *Light_new(Point *position, double radius, Color lightColor);

#endif //SCENE_H