#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"raytracer.h"

#define SHADOW_SAMPLES 20


Point *Surface_intersection(Surface *surface, Line *l, Triangle **p_t);
Color TraceRayR(Scene *scene, Line *l, int depth);

Color TraceRay(Scene *scene, Line *l){
    return TraceRayR(scene, l, 0);
}

int isInShadow(Scene *scene, Surface *nearSurface, Point *intersectionPoint, Point *lightPoint){
    Vector toLight =  Vector_fromPoints(intersectionPoint, lightPoint);
    Line *shadowRay = Line_init(intersectionPoint, &toLight);
    for (int i = 0; i < scene->numSurfaces; i++) {
        if (scene->surfaces[i] == nearSurface || scene->surfaces[i] == NULL || scene->surfaces[i]->type == LIGHT) continue;
        Triangle *t;
        Point *p = Surface_intersection(scene->surfaces[i], shadowRay, &t);
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

Color TraceRayR(Scene *scene, Line *l, int depth){
    Triangle *t;
    Surface *nearSurface = NULL;
    double minDistance = -1;
    Point *intersectionPoint = NULL;
    Triangle *intersectionTriangle = NULL;
    Point *lightPosition = scene->lightSource->position;

    for(int i = 0; i < scene->numSurfaces; i++){
        if(scene->surfaces[i] == NULL) continue;
        Point *p = Surface_intersection(scene->surfaces[i], l, &t);
        if(p != NULL){
            double distance = Point_distanceSquared(l->p, p);
            if(minDistance == -1 || distance < minDistance){
                nearSurface = scene->surfaces[i];
                intersectionPoint = p;
                intersectionTriangle = t;
                minDistance = distance;
            }
        }
    }
    if(nearSurface == NULL) return Color_multiply(BACKGROUND_COLOR, scene->lightSource->color);
    if (nearSurface->type == LIGHT) return intersectionTriangle->color;

    Vector vectorLight = Vector_fromPoints(intersectionPoint, lightPosition);
    vectorLight = Vector_normalize(&vectorLight);

    Vector normal;
    if(nearSurface->type == SPHERE){
        normal = Vector_fromPoints(nearSurface->center, intersectionPoint);
    }
    else{
        normal = Triangle_getNormal(intersectionTriangle);
    }
    normal = Vector_normalize(&normal);
    if(Vector_dot(&normal, l->v) > 0){
        normal = Vector_scale(&normal, -1);
    }

    double epsilon = 1e-3;
    Vector offset = Vector_scale(&normal, epsilon);
    Point *rayOrigin = Point_translate(intersectionPoint, &offset);
    int inShadow = 0;
    double shadowFactor = 1;
    if(scene->lightSource->radius > 0){
        Vector e1 = Vector_perpendicular(&vectorLight);
        e1 = Vector_normalize(&e1);
        e1 = Vector_scale(&e1, scene->lightSource->radius * 1.2);

        //check if the intersection point can be in shadow
        int numCheck = 8;
        double angle = 2 * M_PI / numCheck;
        for(int i = 0; i < numCheck; i++){
            e1 = Vector_rotate(&e1, &vectorLight, angle);
            if(isInShadow(scene, nearSurface, rayOrigin, Point_translate(lightPosition, &e1))){
                inShadow = 1;
                break;
            }
        }
        e1 = Vector_perpendicular(&vectorLight);
        e1 = Vector_normalize(&e1);
        if(inShadow){
            int occluded = 0;
            int numSamples = scene->lightSource->radius == 0 ? 1 : SHADOW_SAMPLES;
            for (int s = 0; s < numSamples; s++) {
                double theta = ((double)rand() / RAND_MAX) * 2 * M_PI;
                double r = scene->lightSource->radius * sqrt((double)rand() / RAND_MAX);

                Vector randomTraslation = Vector_rotate(&e1, &vectorLight, theta);
                randomTraslation = Vector_scale(&randomTraslation, r);
                
                Point *randomLightPoint = Point_translate(lightPosition, &randomTraslation);

                occluded += isInShadow(scene, nearSurface, rayOrigin, randomLightPoint);
            }
            shadowFactor = 1.0 - ((double)occluded / numSamples);
        }
    }
    else{
        shadowFactor = 1 - isInShadow(scene, nearSurface, rayOrigin, lightPosition);
    }

    shadowFactor = __max(shadowFactor, 0.1);

    Vector N = normal;
    Vector L = vectorLight;
    Vector oppositeDirection = Vector_scale(l->v, -1);
    Vector V = Vector_normalize(&oppositeDirection);
    Vector R;

    double diffuseStrength = __max(0.1, Vector_dot(&N, &L));

    double NdotL = Vector_dot(&N, &L);
    Vector tempN = N;
    tempN = Vector_scale(&tempN, 2 * NdotL);
    L = Vector_scale(&L, -1);
    R = Vector_sum(&tempN, &L);
    R = Vector_normalize(&R);

    int shininess = 32;
    double spec = pow(__max(Vector_dot(&R, &V), 0.0), shininess);
    double specularStrength = nearSurface->smoothness;
    
    Color diffuseColor = Color_scale(Color_multiply(intersectionTriangle->color, scene->lightSource->color), diffuseStrength * shadowFactor);
    Color specularColor = Color_scale(COLOR_WHITE, specularStrength * spec * shadowFactor);

    if (nearSurface->reflexivity > 0 && depth < MAX_DEPTH) {
        Vector dir = Vector_normalize(l->v);
        Vector tang = Vector_scale(&normal, -2 * Vector_dot(&normal, &dir));
        Vector reflex = Vector_sum(&dir, &tang);
        double epsilon = 1e-5;
        Vector delta = Vector_scale(&normal, epsilon);

        Line *reflexLine = Line_init(Point_translate(intersectionPoint, &delta), &reflex);
        Color reflectedColor = TraceRayR(scene, reflexLine, depth + 1);
        reflectedColor = Color_scale(reflectedColor, 0.95); // a surface cannot reflect 100% of the light it absorbs
        diffuseColor = Color_blend(diffuseColor, reflectedColor, nearSurface->reflexivity);
    }
    Color finalColor = Color_add(diffuseColor, specularColor);
    return finalColor;
}

Point *intersectionPoint(Line *l, Triangle *t){
    Point *A, *B, *C;
    A = t->a;
    B = t->b;
    C = t->c;
    double EPSILON = 1e-8;
    Vector e1 = Vector_fromPoints(A, B);
    Vector e2 = Vector_fromPoints(A, C);

    Vector h = Vector_crossProduct(l->v, &e2);
    double a = Vector_dot(&e1, &h);
    if (fabs(a) < EPSILON) {
        return NULL;
    }

    Vector s = Vector_fromPoints(A, l->p);
    double u = Vector_dot(&s, &h) / a;
    if (u < 0.0 || u > 1.0) {
        return NULL;
    }

    Vector q = Vector_crossProduct(&s, &e1);
    double v = Vector_dot(l->v, &q) / a;
    if (v < 0.0 || u + v > 1.0) {
        return NULL;
    }

    double ti = Vector_dot(&e2, &q) / a;
    //if intersection is in the opposite direction of the line exclude it
    if (ti < 1e-6) return NULL;


    Point *intersectionPoint = Point_init(0, 0, 0);
    Point *P0 = l->p;
    Vector *D = l->v;
    intersectionPoint->x = P0->x + ti * D->x;
    intersectionPoint->y = P0->y + ti * D->y;
    intersectionPoint->z = P0->z + ti * D->z;

    return intersectionPoint;
}

Point *Sphere_intersection(Surface *sphere, Line *l) {
    //if(sphere->type != SPHERE) return NULL;
    Point *O = l->p;           // Ray origin
    Vector *D = l->v;          // Ray direction (not necessarily normalized)
    Point *C = sphere->center; // Sphere center
    double r = sphere->maxDistanceFromCenter;

    // Vector L = O - C
    Vector L = Vector_init(O->x - C->x, O->y - C->y, O->z - C->z);

    // Compute a, b, c
    double a = Vector_dot(D, D);
    double b = 2 * Vector_dot(&L, D);
    double c = Vector_dot(&L, &L) - r * r;

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
    Point *intersection = Point_init(
        O->x + t * D->x,
        O->y + t * D->y,
        O->z + t * D->z
    );

    return intersection;
}

Point *Surface_intersection(Surface *surface, Line *l, Triangle **p_t){
    *p_t = NULL;
    if(surface->type == SPHERE || surface->type == LIGHT){
        *p_t = surface->triangles[0];
        return Sphere_intersection(surface, l);
    }
    double linePointDistance = Line_Point_distance(l, surface->center);
    if(linePointDistance > surface->maxDistanceFromCenter){
        return NULL;
    }

    Point *q = Line_projectionPoint(l, surface->center);
    Vector pq = Vector_fromPoints(l->p, q);
    if(Vector_dot(&pq, l->v) + surface->maxDistanceFromCenter < 0){
        return NULL;
    }

    Point *intersection = NULL;
    double minDistance = -1;
    for(int i = 0; i < surface->numTriangles; i++){
        Point *p = intersectionPoint(l, surface->triangles[i]);
        if(p != NULL){
            double distance = Point_distanceSquared(l->p, p);
            if(minDistance == -1 || distance < minDistance){
                intersection = p;
                *p_t = surface->triangles[i];
                minDistance = distance;
            }
        }
    }
    return intersection;
}