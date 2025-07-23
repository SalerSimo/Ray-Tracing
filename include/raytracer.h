#ifndef RAYTRACER_H
#define RAYTRACER_H

#include"geometry.h"
#include"surface.h"
#include"scene.h"
#include"color.h"

Color TraceRay(Scene *scene, Line *l, int depth);

#endif //RAYTRACER_H