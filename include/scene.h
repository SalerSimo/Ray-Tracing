#ifndef SCENE_H
#define SCENE_H

#include<stdio.h>
#include<stdlib.h>

#include"geometry.h"
#include"surface.h"

typedef struct{
    Point *camera;
    double cameraDistance;
    double rotationAngle;
    Surface **surfaces;
    int numSurfaces;
    Point *lightSource;
}Scene;

Scene *Scene_init();
void Scene_fill(Scene *s, Point *lightSource, Surface **surfaces, int numSurfaces);

#endif //SCENE_H