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

void Scene_fill(Scene *s, Light *lightSource, Surface **surfaces, int numSurfaces){
    if(s->numSurfaces > 0){
        free(s->surfaces);
    }
    s->lightSource = lightSource;
    s->numSurfaces = numSurfaces + 1;
    s->surfaces = malloc((numSurfaces + 1) * sizeof(Surface));
    for(int i = 0; i < numSurfaces; i++){
        s->surfaces[i] = surfaces[i];
    }
    s->surfaces[numSurfaces] = Surface_createSphere(lightSource->position, lightSource->radius, 0, 0, lightSource->color);
    s->surfaces[numSurfaces]->type = LIGHT;
}

Light *Light_new(Point *position, double radius, Color lightColor){
    Light *light = malloc(sizeof(Light));
    light->position = position;
    light->radius = radius;
    light->color = lightColor;
    return light;
}