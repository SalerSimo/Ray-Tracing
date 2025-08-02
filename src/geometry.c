#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"geometry.h"

Vector Vector_init(double x, double y, double z){
    Vector v;
    v.x = x;
    v.y = y;
    v.z = z;
    v.normSquared = x*x + y*y + z*z;
    return v;
}

Vector Vector_fromPoints(Point *a, Point *b){
    return Vector_init(b->x - a->x, b->y - a->y, b->z - a->z);
}

Vector Vector_normalize(Vector v){
    Vector vNorm = Vector_init(v.x, v.y, v.z);
    double norm = sqrt(vNorm.normSquared);
    vNorm.x /= norm;
    vNorm.y /= norm;
    vNorm.z /= norm;
    vNorm.normSquared = 1;
    return vNorm;
}

Vector Vector_scale(Vector v, double scalar){
    return Vector_init(v.x * scalar, v.y * scalar, v.z * scalar);
}

Vector Vector_sum(Vector v1, Vector v2){
    return Vector_init(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

Vector Vector_crossProduct(Vector a, Vector b){
    double x, y, z;
    x = a.y * b.z - a.z * b.y;
    y = a.z * b.x - a.x * b.z;
    z = a.x * b.y - a.y * b.x;
    return Vector_init(x, y, z);
}

double Vector_dot(Vector v1, Vector v2){
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z; 
}

Vector Vector_perpendicular(Vector v){
    Vector e = Vector_init(1, 0, 0);
    if(Vector_equal(v, e)){
        e = Vector_init(0, 1, 0);
    }
    return Vector_crossProduct(v, e);
}

Vector Vector_rotate(Vector v, Vector axis, double angle){
    Vector rotated1, rotated2;
    axis = Vector_normalize(axis);
    rotated1 = Vector_scale(v, cos(angle));
    rotated2 = Vector_scale(Vector_crossProduct(v, axis), sin(angle));
    return Vector_sum(rotated1, rotated2);
}

int Vector_equal(Vector v1, Vector v2){
    return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

void Vector_print(Vector *v){
    printf("%f\n", v->x);
    printf("%f\n", v->y);
    printf("%f\n", v->z);
    printf("\n");
}


Point *Point_init(double x, double y, double z){
    Point *p = malloc(sizeof(Point));
    if(p == NULL) return NULL;
    p->x = x;
    p->y = y;
    p->z = z;
    return p;
}

Point *Point_translate(Point *p, Vector *v){
    return Point_init(p->x + v->x, p->y + v->y, p->z + v->z);
}

void Point_print(Point *p){
    printf("(%f, %f, %f)\n", p->x, p->y, p->z);
}

double Point_distanceSquared(Point *a, Point *b){
    return pow(a->x - b->x, 2) + pow(a->y - b->y, 2) + pow(a->z - b->z, 2);
}

Line *Line_init(Point *p, Vector *v){
    Line *l = malloc(sizeof(Line));
    if(l == NULL) return NULL;
    l->p = p;
    l->v = v;
    return l;
}

void Line_print(Line *l){
    printf("%.2f   %.2f*t\n", l->p->x, l->v->x);
    printf("%.2f + %.2f*t\n", l->p->y, l->v->y);
    printf("%.2f   %.2f*t\n", l->p->z, l->v->z);
}

double Line_Point_distance(Line *l, Point *p){
    Vector p0p1 = Vector_fromPoints(l->p, p);

    Vector v =  Vector_crossProduct(*l->v, p0p1);
    Vector n = Vector_crossProduct(*l->v, v);

    double distance = abs(Vector_dot(p0p1, n)) / sqrt(n.normSquared);
    return distance;
}

Plane *Plane_new(double A, double B, double C, double D){
    Plane *plane = malloc(sizeof(Plane));
    plane->A = A;
    plane->B = B;
    plane->C = C;
    plane->D = D;
}

Vector Plane_vectorNormal(Plane *plane){
    return Vector_init(plane->A, plane->B, plane->C);
}

Point *Line_projectionPoint(Line *l, Point *p){
    Vector v = Vector_fromPoints(l->p, p);
    double scale = Vector_dot(v, *l->v) / l->v->normSquared;
    return Point_init(
        l->p->x + scale*l->v->x,
        l->p->y + scale*l->v->y,
        l->p->z + scale*l->v->z
    );
}
