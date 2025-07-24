#ifndef RAYTRACER_H
#define RAYTRACER_H

#include"geometry.h"
#include"surface.h"
#include"scene.h"
#include"color.h"

#define MAX_DEPTH 3

Color TraceRay(Scene *scene, Line *l);

#endif //RAYTRACER_H