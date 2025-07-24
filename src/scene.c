#include"scene.h"

Scene *Scene_init(){
    Scene *s = malloc(sizeof(Scene*));
    if(s == NULL) return NULL;
    s->numSurfaces = 0;
    s->surfaces = NULL;
    s->rotationAngle = 0;
    s->camera = NULL;
    s->cameraDistance = 0;
    s->lightSource = NULL;
    return s;
}

void Scene_fill(Scene *s, Point *lightSource, Surface **surfaces, int numSurfaces){
    if(s->numSurfaces > 0){
        free(s->surfaces);
    }
    s->lightSource = lightSource;
    s->numSurfaces = numSurfaces;
    s->surfaces = malloc(numSurfaces * sizeof(Surface));
    for(int i = 0; i < numSurfaces; i++){
        s->surfaces[i] = surfaces[i];
    }
}