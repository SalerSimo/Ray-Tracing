#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"raytracer.h"

#define SHADOW_SAMPLES 20


Point *Model_intersection(Model *model, Ray *l, Triangle **p_t);
Color TraceRayR(Scene *scene, Ray *l, int depth);

Color TraceRay(Scene *scene, Ray *ray){
	return TraceRayR(scene, ray, 0);
}

Vector Reflect(Vector incident, Vector normal) {
	return Vector_sum(incident, Vector_scale(normal, -2 * Vector_dot(incident, normal)));
}

int isInShadow(Scene *scene, Model *nearModel, Point *intersectionPoint, Point *lightPoint){
	Vector toLight =  Vector_fromPoints(intersectionPoint, lightPoint);
	Ray *shadowRay = Line_init(intersectionPoint, toLight);
	for (int i = 0; i < scene->numModels; i++) {
		if (scene->models[i] == nearModel || scene->models[i] == NULL || scene->models[i]->type == LIGHT) continue;
		Triangle *t;
		Point *p = Model_intersection(scene->models[i], shadowRay, &t);
		if (p != NULL) {
			double distToObj = Point_distanceSquared(p, intersectionPoint);
			double distToLight = Point_distanceSquared(lightPoint, intersectionPoint);
			if (distToObj < distToLight - 1e-5) {
				return 1;
			}
		}
	}
	return 0;
}

double CalculateShadowFactor(Scene *scene, Model *nearModel, Vector normal, Vector vectorLight, Point *intersectionPoint){
	double epsilon = 1e-3;
	Vector offset = Vector_scale(normal, epsilon);
	Point *rayOrigin = Point_translate(intersectionPoint, offset);
	int inShadow = 0;
	double shadowFactor = 1;
	Light *light = scene->lightSource;
	if(light->radius > 0){
		Vector e1 = Vector_normalize(Vector_perpendicular(vectorLight));
		e1 = Vector_scale(e1, light->radius * 1.2);

		//check if the intersection point can be in shadow
		int numCheck = 8;
		double angle = 2 * M_PI / numCheck;
		for(int i = 0; i < numCheck; i++){
			e1 = Vector_rotate(e1, vectorLight, angle);
			if(isInShadow(scene, nearModel, rayOrigin, Point_translate(light->position, e1))){
				inShadow = 1;
				break;
			}
		}
		e1 = Vector_normalize(Vector_perpendicular(vectorLight));
		if(inShadow){
			int occluded = 0;
			int numSamples = light->radius == 0 ? 1 : SHADOW_SAMPLES;
			for (int s = 0; s < numSamples; s++) {
				double theta = ((double)rand() / RAND_MAX) * 2 * M_PI;
				double r = light->radius * sqrt((double)rand() / RAND_MAX);

				Vector randomTraslation = Vector_rotate(e1, vectorLight, theta);
				randomTraslation = Vector_scale(randomTraslation, r);
				
				Point *randomLightPoint = Point_translate(light->position, randomTraslation);

				occluded += isInShadow(scene, nearModel, rayOrigin, randomLightPoint);
			}
			shadowFactor = 1.0 - ((double)occluded / numSamples);
		}
	}
	else{
		shadowFactor = 1 - isInShadow(scene, nearModel, rayOrigin, light->position);
	}

	shadowFactor = fmax(shadowFactor, 0.1);
	return shadowFactor;
}

Color TraceRayR(Scene *scene, Ray *ray, int depth){
	Triangle *t;
	Model *nearModel = NULL;
	double minDistance = -1;
	Point *intersectionPoint = NULL;
	Triangle *intersectionTriangle = NULL;
	Point *lightPosition = scene->lightSource->position;

	Light *light = scene->lightSource;

	for(int i = 0; i < scene->numModels; i++){
		if(scene->models[i] == NULL) continue;
		if(intersectionPoint != NULL && Vector_dot(Vector_fromPoints(scene->models[i]->center, intersectionPoint), ray->direction) < 0 && Point_distanceSquared(scene->models[i]->center, intersectionPoint) > scene->models[i]->maxDistanceFromCenter * scene->models[i]->maxDistanceFromCenter){
			continue;
		}
		Point *p = Model_intersection(scene->models[i], ray, &t);
		if(p != NULL){
			double distance = Point_distanceSquared(ray->origin, p);
			if(minDistance == -1 || distance < minDistance){
				nearModel = scene->models[i];
				intersectionPoint = p;
				intersectionTriangle = t;
				minDistance = distance;
			}
		}
	}
	if(nearModel == NULL) return Color_multiply(BACKGROUND_COLOR, light->color);
	if (nearModel->type == LIGHT) return intersectionTriangle->color;

	Vector vectorLight = Vector_normalize(Vector_fromPoints(intersectionPoint, lightPosition));

	Vector normal;
	if(nearModel->type == SPHERE)
		normal = Vector_fromPoints(nearModel->center, intersectionPoint);
	else
		normal = Triangle_getNormal(intersectionTriangle);
	normal = Vector_normalize(normal);
	if(Vector_dot(normal, ray->direction) > 0)
		normal = Vector_scale(normal, -1);

	double shadowFactor = CalculateShadowFactor(scene, nearModel, normal, vectorLight, intersectionPoint);

	Vector oppositeDirection = Vector_normalize(Vector_scale(ray->direction, -1));
	Vector R;

	double diffuseStrength = fmax(0.1, Vector_dot(normal, vectorLight));

	Vector tempN = Vector_scale(normal, 2 * Vector_dot(normal, vectorLight));
	R = Vector_normalize(Vector_sum(tempN, Vector_scale(vectorLight, -1)));

	int shininess = 32;
	double spec = pow(fmax(Vector_dot(R, oppositeDirection), 0.0), shininess);
	double specularStrength = nearModel->shininess;
	
	Color diffuseColor = Color_scale(Color_multiply(intersectionTriangle->color, light->color), diffuseStrength * shadowFactor);
	Color specularColor = Color_scale(COLOR_WHITE, specularStrength * spec * shadowFactor);

	if (nearModel->reflexivity > 0 && depth < MAX_DEPTH) {
		Vector reflex = Reflect(ray->direction, normal);
		double epsilon = 1e-5;
		Vector delta = Vector_scale(normal, epsilon);

		Ray *reflexRay = Line_init(Point_translate(intersectionPoint, delta), reflex);
		Color reflectedColor = TraceRayR(scene, reflexRay, depth + 1);
		reflectedColor = Color_scale(reflectedColor, 0.95); // a model cannot reflect 100% of the light it absorbs
		diffuseColor = Color_blend(diffuseColor, reflectedColor, nearModel->reflexivity);
	}
	Color finalColor = Color_add(diffuseColor, specularColor);


	double distanceSquared = Point_distanceSquared(intersectionPoint, lightPosition);
	double attenuation = light->constant + light->linear * sqrt(distanceSquared) + light->quadratic * distanceSquared;
	attenuation = 1 / attenuation;

	return Color_scale(finalColor, attenuation);
}

Point *intersectionPoint(Ray *ray, Triangle *t){
	Point *A, *B, *C;
	A = t->a;
	B = t->b;
	C = t->c;
	double EPSILON = 1e-8;
	Vector e1 = Vector_fromPoints(A, B);
	Vector e2 = Vector_fromPoints(A, C);

	Vector h = Vector_crossProduct(ray->direction, e2);
	double a = Vector_dot(e1, h);
	if (fabs(a) < EPSILON) {
		return NULL;
	}

	Vector s = Vector_fromPoints(A, ray->origin);
	double u = Vector_dot(s, h) / a;
	if (u < 0.0 || u > 1.0) {
		return NULL;
	}

	Vector q = Vector_crossProduct(s, e1);
	double v = Vector_dot(ray->direction, q) / a;
	if (v < 0.0 || u + v > 1.0) {
		return NULL;
	}

	double ti = Vector_dot(e2, q) / a;
	//if intersection is in the opposite direction of the line exclude it
	if (ti < 1e-6) return NULL;


	Point* intersectionPoint = Point_translate(ray->origin, Vector_scale(ray->direction, ti));

	return intersectionPoint;
}

Point *Sphere_intersection(Model *sphere, Ray *ray) {
	Point *O = ray->origin;
	Vector D = ray->direction;
	Point *C = sphere->center;
	double r = sphere->maxDistanceFromCenter;

	Vector L = Vector_fromPoints(C, O);

	// Compute a, b, c
	double a = Vector_dot(D, D);
	double b = 2 * Vector_dot(L, D);
	double c = Vector_dot(L, L) - r * r;

	double discriminant = b * b - 4 * a * c;

	if (discriminant < 0) {
		// No intersection
		return NULL;
	}

	double sqrt_discriminant = sqrt(discriminant);
	double t1 = (-b - sqrt_discriminant) / (2 * a);
	double t2 = (-b + sqrt_discriminant) / (2 * a);

	double t;
	if (t1 > 0) t = t1;
	else if (t2 > 0) t = t2;
	else return NULL; // Both intersections are behind the camera

	// Compute intersection point: P = O + t * D
	Point* intersection = Point_translate(O, Vector_scale(D, t));

	return intersection;
}

Point *Model_intersection(Model *model, Ray *ray, Triangle **p_t){
	*p_t = NULL;
	if(model->type == SPHERE || model->type == LIGHT){
		*p_t = model->triangles[0];
		return Sphere_intersection(model, ray);
	}
	double linePointDistance = Line_Point_distance(ray, model->center);
	if(linePointDistance > model->maxDistanceFromCenter){
		return NULL;
	}

	Point *q = Line_projectionPoint(ray, model->center);
	Vector pq = Vector_fromPoints(ray->origin, q);
	if(Vector_dot(pq, ray->direction) + model->maxDistanceFromCenter < 0){
		return NULL;
	}

	Point *intersection = NULL;
	double minDistance = -1;
	for(int i = 0; i < model->numTriangles; i++){
		Point *p = intersectionPoint(ray, model->triangles[i]);
		if(p != NULL){
			double distance = Point_distanceSquared(ray->origin, p);
			if(minDistance == -1 || distance < minDistance){
				intersection = p;
				*p_t = model->triangles[i];
				minDistance = distance;
			}
		}
	}
	return intersection;
}