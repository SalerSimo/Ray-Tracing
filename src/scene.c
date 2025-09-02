#include"scene.h"
#include<math.h>

Scene *Scene_init(Point *camera, double fieldOfView){
    Scene *s = malloc(sizeof(Scene*));
    if(s == NULL) return NULL;
    s->numSurfaces = 0;
    s->surfaces = NULL;
    s->rotationAngle = 0;
    s->camera = camera;
    if(fieldOfView < 0 || fieldOfView >= M_PI) s->fieldOfView = 90 * M_PI / 180;
    else s->fieldOfView = fieldOfView;
    s->lightSource = NULL;
    return s;
}

void Scene_fill(Scene *s, Light *lightSource, Surface **surfaces, int numSurfaces){
    if(s->numSurfaces > 0){
        free(s->surfaces);
    }
    s->lightSource = lightSource;
    s->numSurfaces = numSurfaces + 1;
    if(surfaces == NULL){
        s->numSurfaces = 1;
    }
    s->surfaces = malloc((s->numSurfaces) * sizeof(Surface));
    for(int i = 0; i < s->numSurfaces - 1; i++){
        s->surfaces[i] = surfaces[i];
    }
    s->surfaces[s->numSurfaces - 1] = Surface_createSphere(lightSource->position, lightSource->radius, 0, 0, lightSource->color);
    s->surfaces[s->numSurfaces - 1]->type = LIGHT;
}

void Scene_addSurfaces(Scene *s, Surface **surfaces, int numSurfaces){
    if(surfaces == NULL || numSurfaces < 1) return;
    int totSurfaces = s->numSurfaces + numSurfaces;
    Surface **newSurfaces = malloc(totSurfaces * sizeof(Surface*));

    for(int i = 0; i < s->numSurfaces; i++){
        newSurfaces[i] = s->surfaces[i];
    }
    for(int i = 0; i < numSurfaces; i++){
        newSurfaces[i + s->numSurfaces] = surfaces[i];
    }

    free(s->surfaces);
    s->numSurfaces = totSurfaces;
    s->surfaces = newSurfaces;
}

Light *Light_new(Point *position, double radius, Color lightColor){
    Light *light = malloc(sizeof(Light));
    light->position = position;
    light->radius = radius;
    light->color = lightColor;

    light->constant = 1;
    light->linear = 0.0009;
    light->quadratic = 0.000032;
    return light;
}