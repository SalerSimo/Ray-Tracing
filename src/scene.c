#include"scene.h"
#include<math.h>

Scene *Scene_init(Camera *camera){
	Scene *s = malloc(sizeof(Scene*));
	if(s == NULL){
		printf("ERROR::SCENE::Memory allocation failed.\n");
		return NULL;
	}
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

	Scene_sortModels(s);
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
	Scene_sortModels(s);
}

void Scene_sortModels(Scene *s){
	int min;
	Point *cameraPosition = s->camera->position;
	for(int i = 0; i < s->numModels; i++){
		min = i;
		for(int j = i + 1; j < s->numModels; j++){
			if(Point_distanceSquared(cameraPosition, s->models[j]->center) < Point_distanceSquared(cameraPosition, s->models[min]->center)){
				min = j;
			}
		}
		Model *tmp = s->models[i];
		s->models[i] = s->models[min];
		s->models[min] = tmp;
	}
}

Light *Light_new(Point *position, double radius, Color lightColor){
	Light *light = malloc(sizeof(Light));
	light->position = position;
	light->radius = radius;
	light->color = lightColor;

	light->constant = 1;
	light->linear = 0;
	light->quadratic = 0;
	return light;
}

void Light_setAttenuation(Light *light, double constant, double linear, double quadratic){
	if(light == NULL) return;
	light->constant = constant;
	light->linear = linear;
	light->quadratic = quadratic;
}