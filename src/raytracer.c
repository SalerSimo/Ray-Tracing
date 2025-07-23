#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"raytracer.h"

Point *Surface_intersection(Surface *surface, Line *l, Triangle **p_t);

Color TraceRay(Scene *scene, Line *l, int depth){
    if(Line_Point_distance(l, scene->lightSource) <= 0.005 * Point_distance(l->p, scene->lightSource)){
        Vector v = Vector_fromPoints(l->p, scene->lightSource);
        if(Vector_dot(&v, l->v) > 0){
            return COLOR_WHITE;
        }
    }
    Triangle *t;
    Surface *nearSurface = NULL;
    double minDistance = -1;
    Point *intersectionPoint = NULL;
    Triangle *intersectionTriangle = NULL;
    Vector normal;

    for(int i = 0; i < scene->numSurfaces; i++){
        if(scene->surfaces[i] == NULL) continue;
        Point *p = Surface_intersection(scene->surfaces[i], l, &t);
        if(p != NULL){
            double distance = Point_distance(l->p, p);
            if(minDistance == -1 || distance < minDistance){
                nearSurface = scene->surfaces[i];
                intersectionPoint = p;
                intersectionTriangle = t;
                minDistance = distance;
            }
        }
    }
    if(nearSurface == NULL) return COLOR_BLACK;

    Vector vectorLight = Vector_fromPoints(intersectionPoint, scene->lightSource);
    vectorLight = Vector_normalize(&vectorLight);

    int inShadow = 0;
    for(int i = 0; i < scene->numSurfaces; i++){
        if(scene->surfaces[i] == nearSurface || scene->surfaces[i] == NULL) continue;
        Line *lightLine =  Line_init(intersectionPoint, &vectorLight);
        Point *p = Surface_intersection(scene->surfaces[i], lightLine, &t);
        if(p != NULL){
            Vector v = Vector_fromPoints(lightLine->p, p);
            if (Vector_dot(lightLine->v, &v) > 0 && Point_distance(p, intersectionPoint) < Point_distance(scene->lightSource, intersectionPoint)) {
                inShadow = 1;
            }
        }
    }

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
    double factor = __max(0.1, Vector_dot(&vectorLight, &normal));

    Color reflectedColor = COLOR_BLACK;
    uint32_t color;

    Vector N = normal;
    Vector L = vectorLight;
    Vector oppositeDirection = Vector_scale(l->v, -1);
    Vector V = Vector_normalize(&oppositeDirection);
    Vector R;

    double diffuseStrength = __max(0.0, Vector_dot(&N, &L));

    double NdotL = Vector_dot(&N, &L);
    Vector tempN = N;
    tempN = Vector_scale(&tempN, 2 * NdotL);
    L = Vector_scale(&L, -1);
    R = Vector_sum(&tempN, &L);
    R = Vector_normalize(&R);

    int shininess = 32;
    double spec = pow(__max(Vector_dot(&R, &V), 0.0), shininess);
    double specularStrength = nearSurface->smoothness;
    
    Color diffuseColor = inShadow ? Color_scale(nearSurface->color, 0.1) : Color_scale(nearSurface->color, diffuseStrength);
    Color specularColor = inShadow ? COLOR_BLACK : Color_scale(COLOR_WHITE, specularStrength * spec);
    Color lightingColor = Color_add(diffuseColor, specularColor);
    Color finalColor;

    if (nearSurface->reflexivity > 0 && depth < MAX_DEPTH) {
        Vector dir = Vector_normalize(l->v);
        Vector tang = Vector_scale(&normal, -2 * Vector_dot(&normal, &dir));
        Vector reflex = Vector_sum(&dir, &tang);
        double epsilon = 1e-2;
        Vector delta = Vector_scale(&normal, epsilon);

        Line *reflexLine = Line_init(Point_traslate(intersectionPoint, &delta), &reflex);
        reflectedColor = TraceRay(scene, reflexLine, depth + 1);
        finalColor = Color_blend(lightingColor, reflectedColor, nearSurface->reflexivity);
    }
    else{
        finalColor = lightingColor;
    }
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
    if(sphere->type != SPHERE) return NULL;
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
    if(surface->type == SPHERE){
        return Sphere_intersection(surface, l);
    }
    double d = Point_distance(l->p, surface->center);
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
            double distance = Point_distance(l->p, p);
            if(minDistance == -1 || distance < minDistance){
                intersection = p;
                *p_t = surface->triangles[i];
                minDistance = distance;
            }
        }
    }
    return intersection;
}