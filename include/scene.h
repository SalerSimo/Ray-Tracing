#ifndef SCENE_H
#define SCENE_H

#include<stdio.h>
#include<stdlib.h>

#include"geometry.h"
#include"model.h"
#include"camera.h"

typedef struct{
    Point *position;
    double radius;
    Color color;

    double constant, linear, quadratic;
}Light;

/**
 * Represents a 3D scene for ray tracing.
 *
 * Contains camera position, light source, and an array of models that define the geometry.
 * It also includes camera-related properties like the viewing distance and rotation angle.
 */
typedef struct{
    /** Pointer to the camera object in the scene */
    Camera *camera;
    /** Array of pointers to the models in the scene. */
    Model **models;
    /** Number of models in the scene. */
    unsigned int numModels;
    /** Pointer to the light source of the scene. */
    Light *lightSource;
}Scene;


/**
 * @brief Creates an empty scene.
 * 
 * It sets the camera and the field of view (FOV), if the passed FOV is invalid, the FOV of the scene is set to 90°.
 * It also sets all other pointers to NULL and all numeric values to 0.
 * 
 * @param camera Pointer to the camera position.
 * 
 * @return Pointer to the allocated scene or NULL if allocation fails.
 */
Scene *Scene_init(Camera *camera);

/**
 * Initializes a scene with the given light source and models.
 * 
 * @param s The scene to fill.
 * @param lightSource Pointer to the position of the light source.
 * @param models Array of pointers to the models to include in the scene.
 * @param numModels Number of models in the array.
 */
void Scene_fill(Scene *s, Light *lightSource, Model **models, int numModels);


/**
 * @brief Adds multiple models to the scene.
 *
 *
 * @param s Pointer to the Scene object to which models will be added.
 * @param models Array of pointers to Model objects to be added.
 * @param numModels Number of Models in the array.
 *
 * @warning This function does not prevent duplicate Model pointers from being added.
 */
void Scene_addModels(Scene *s, Model **models, int numModels);


Light *Light_new(Point *position, double radius, Color lightColor);

#endif //SCENE_H