#include<stdlib.h>
#include<math.h>
#include<stdbool.h>
#include"raytracer.h"

#define SHADOW_SAMPLES 20

typedef struct{
	Point* point;
	Vector normal;
	Material material;

	Model *model;
}Hit;


Hit Model_intersection(Model *model, Ray *l, bool triangleSorted);
Color TraceRayR(Scene *scene, Ray *l, int depth);

Color TraceRay(Scene *scene, Ray *ray){
	return TraceRayR(scene, ray, 0);
}

Vector Reflect(Vector incident, Vector normal) {
	return Vector_sum(incident, Vector_scale(normal, -2 * Vector_dot(incident, normal)));
}

int isInShadow(Scene *scene, Hit realHit, Point *lightPoint){
	Vector toLight =  Vector_fromPoints(realHit.point, lightPoint);
	Ray *shadowRay = Line_init(realHit.point, toLight);
	for (int i = 0; i < scene->numModels; i++) {
		if (scene->models[i] == realHit.model || scene->models[i] == NULL || scene->models[i]->type == LIGHT) continue;
		Hit hit = Model_intersection(scene->models[i], shadowRay, false);
		if (hit.point != NULL) {
			double distToObj = Point_distanceSquared(hit.point, realHit.point);
			double distToLight = Point_distanceSquared(lightPoint, hit.point);
			if (distToObj < distToLight - 1e-5) {
				return 1;
			}
		}
	}
	return 0;
}

double CalculateShadowFactor(Scene *scene, Hit realHit, Vector vectorLight){
	double epsilon = 1e-3;
	Vector offset = Vector_scale(realHit.normal, epsilon);
	realHit.point = Point_translate(realHit.point, offset);
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
			if(isInShadow(scene, realHit, Point_translate(light->position, e1))){
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

				occluded += isInShadow(scene, realHit, randomLightPoint);
			}
			shadowFactor = 1.0 - ((double)occluded / numSamples);
		}
	}
	else{
		shadowFactor = 1 - isInShadow(scene, realHit, light->position);
	}

	shadowFactor = fmax(shadowFactor, 0.1);
	return shadowFactor;
}

Color TraceRayR(Scene *scene, Ray *ray, int depth){
	double minDistance = -1;

	Light *light = scene->lightSource;
	Hit realHit;
	realHit.point = NULL;

	for(int i = 0; i < scene->numModels; i++){
		if(scene->models[i] == NULL) continue;
		if(realHit.point != NULL && Vector_dot(Vector_fromPoints(scene->models[i]->center, realHit.point), ray->direction) < 0 && Point_distanceSquared(scene->models[i]->center, realHit.point) > scene->models[i]->maxDistanceFromCenter * scene->models[i]->maxDistanceFromCenter){
			continue;
		}
		bool sorted = false;
		if(depth == 0) sorted = true;
		Hit currentHit = Model_intersection(scene->models[i], ray, sorted);
		if(currentHit.point != NULL){
			double distance = Point_distanceSquared(ray->origin, currentHit.point);
			if(minDistance == -1 || distance < minDistance){
				realHit = currentHit;
				minDistance = distance;
			}
		}
	}
	if(realHit.point == NULL) return Color_multiply(BACKGROUND_COLOR, light->color);
	if (realHit.model->type == LIGHT) return realHit.material.color;

	Vector vectorLight = Vector_normalize(Vector_fromPoints(realHit.point, light->position));

	if(Vector_dot(realHit.normal, ray->direction) > 0)
		realHit.normal = Vector_scale(realHit.normal, -1);

	realHit.normal = Vector_normalize(realHit.normal);

	double shadowFactor = CalculateShadowFactor(scene, realHit, vectorLight);

	Vector oppositeDirection = Vector_normalize(Vector_scale(ray->direction, -1));

	double diffuseStrength = fmax(0.1, Vector_dot(realHit.normal, vectorLight));

	Vector tempN = Vector_scale(realHit.normal, 2 * Vector_dot(realHit.normal, vectorLight));
	Vector R = Vector_normalize(Vector_sum(tempN, Vector_scale(vectorLight, -1)));

	int shininess = 32;
	double spec = pow(fmax(Vector_dot(R, oppositeDirection), 0.0), shininess);
	double specularStrength = realHit.material.shininess;
	
	Color diffuseColor = Color_scale(Color_multiply(realHit.material.color, light->color), diffuseStrength * shadowFactor);
	Color specularColor = Color_scale(COLOR_WHITE, specularStrength * spec * shadowFactor);

	if (realHit.material.reflexivity > 0 && depth < MAX_DEPTH) {
		Vector reflex = Reflect(ray->direction, realHit.normal);
		double epsilon = 1e-5;
		Vector delta = Vector_scale(realHit.normal, epsilon);

		Ray *reflexRay = Line_init(Point_translate(realHit.point, delta), reflex);
		Color reflectedColor = TraceRayR(scene, reflexRay, depth + 1);
		reflectedColor = Color_scale(reflectedColor, 0.95); // a model cannot reflect 100% of the light it absorbs
		diffuseColor = Color_blend(diffuseColor, reflectedColor, realHit.material.reflexivity);
	}
	Color finalColor = Color_add(diffuseColor, specularColor);


	double distanceSquared = Point_distanceSquared(realHit.point, light->position);
	double attenuation = light->constant + light->linear * sqrt(distanceSquared) + light->quadratic * distanceSquared;
	attenuation = 1 / attenuation;

	return Color_scale(finalColor, attenuation);
}

Point *intersection_Point(Ray *ray, Triangle *t){
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

Hit Sphere_intersection(Model *sphere, Ray *ray) {
	Hit hit;
	hit.point = NULL;
	Point *O = ray->origin;
	Vector D = ray->direction;
	Point *C = sphere->center;
	double r = fmax(0.1, sphere->maxDistanceFromCenter);

	Vector L = Vector_fromPoints(C, O);

	// Compute a, b, c
	double a = Vector_dot(D, D);
	double b = 2 * Vector_dot(L, D);
	double c = Vector_dot(L, L) - r * r;

	double discriminant = b * b - 4 * a * c;

	if (discriminant < 0) {
		// No intersection
		return hit;
	}

	double sqrt_discriminant = sqrt(discriminant);
	double t1 = (-b - sqrt_discriminant) / (2 * a);
	double t2 = (-b + sqrt_discriminant) / (2 * a);

	double t;
	if (t1 > 0) t = t1;
	else if (t2 > 0) t = t2;
	else return hit; // Both intersections are behind the camera

	// Compute intersection point: P = O + t * D
	Point* intersection = Point_translate(O, Vector_scale(D, t));

	hit.point = intersection;
	hit.normal = Vector_fromPoints(sphere->center, intersection);
	hit.model = sphere;
	Material mat;
	mat.color = sphere->triangles[0]->color;
	mat.reflexivity = sphere->reflexivity;
	mat.shininess = sphere->shininess;
	hit.material = mat;

	return hit;
}

Hit Model_intersection(Model *model, Ray *ray, bool triangleSorted){
	if(model->type == SPHERE || model->type == LIGHT){
		return Sphere_intersection(model, ray);
	}
	Hit hit;
	hit.point = NULL;
	double linePointDistance = Line_Point_distance(ray, model->center);
	if(linePointDistance > model->maxDistanceFromCenter){
		return hit;
	}

	Point *q = Line_projectionPoint(ray, model->center);
	Vector pq = Vector_fromPoints(ray->origin, q);
	if(Vector_dot(pq, ray->direction) + model->maxDistanceFromCenter < 0){
		return hit;
	}

	Point *intersection = NULL;
	double minDistance = -1;
	Triangle *hitTriangle = NULL;
	for(int i = 0; i < model->numTriangles; i++){
		Point *p = intersection_Point(ray, model->triangles[i]);
		if(p != NULL){
			double distance = Point_distanceSquared(ray->origin, p);
			if(minDistance == -1 || distance < minDistance){
				intersection = p;
				hitTriangle = model->triangles[i];
				minDistance = distance;
				if(triangleSorted) break;
			}
		}
	}

	if(intersection == NULL) return hit;

	hit.point = intersection;
	hit.normal = Triangle_getNormal(hitTriangle);
	hit.model = model;
	Material mat;
	mat.color = (hitTriangle)->color;
	mat.reflexivity = model->reflexivity;
	mat.shininess = model->shininess;
	hit.material = mat;
	return hit;
}