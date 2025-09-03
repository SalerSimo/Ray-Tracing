#ifndef RAYTRACER_H
#define RAYTRACER_H

#include"geometry.h"
#include"model.h"
#include"scene.h"
#include"color.h"

#define MAX_DEPTH 3
#define BACKGROUND_COLOR Color_new(0xA7ECFF)

typedef Line Ray;

/**
 * Traces a single ray in the scene and returns the resulting color.
 *
 * It computes the color seen along a given ray by checking for model intersections, shading, and reflections.
 *
 * @param scene Pointer to the scene containing models and the light source.
 * @param l Pointer to the ray (Line) to trace.
 * @return The computed Color seen along the ray.
 */
Color TraceRay(Scene *scene, Line *l);

#endif //RAYTRACER_H