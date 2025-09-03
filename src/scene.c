#include"scene.h"
#include<math.h>

Scene *Scene_init(Camera *camera){
    Scene *s = malloc(sizeof(Scene*));
    if(s == NULL) return NULL;
    s->numModels = 0;
    s->models = NULL;;
    s->camera = camera;
    s->lightSource = NULL;
    return s;
}

void Scene_fill(Scene *s, Light *lightSource, Model **models, int numModels){
    if(s->numModels > 0){
        free(s->models);
    }
    s->lightSource = lightSource;
    s->numModels = numModels + 1;
    if(models == NULL){
        s->numModels = 1;
    }
    s->models = malloc((s->numModels) * sizeof(Model));
    for(int i = 0; i < s->numModels - 1; i++){
        s->models[i] = models[i];
    }
    s->models[s->numModels - 1] = Model_createSphere(lightSource->position, lightSource->radius, 0, 0, lightSource->color);
    s->models[s->numModels - 1]->type = LIGHT;
}

void Scene_addModels(Scene *s, Model **models, int numModels){
    if(models == NULL || numModels < 1) return;
    int totModels = s->numModels + numModels;
    Model **newModels = malloc(totModels * sizeof(Model*));

    for(int i = 0; i < s->numModels; i++){
        newModels[i] = s->models[i];
    }
    for(int i = 0; i < numModels; i++){
        newModels[i + s->numModels] = models[i];
    }

    free(s->models);
    s->numModels = totModels;
    s->models = newModels;
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