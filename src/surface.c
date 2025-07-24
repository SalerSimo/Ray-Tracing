#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<stdint.h>
#include"surface.h"

Triangle *Triangle_init(Point *a, Point *b, Point *c){
    Triangle *t = malloc(sizeof(Triangle));
    t->a = a;
    t->b = b;
    t->c = c;
    return t;
}

double Triangle_area(Triangle *t){
    Vector ab, ac;
    ab = Vector_fromPoints(t->a, t->b);
    ac = Vector_fromPoints(t->a, t->c);
    Vector v = Vector_crossProduct(&ab, &ac);
    return v.norm / 2;
}

Vector Triangle_getNormal(Triangle *t){
    Vector ab, ac;
    ab = Vector_fromPoints(t->a, t->b);
    ac = Vector_fromPoints(t->a, t->c);
    Vector normal = Vector_crossProduct(&ab, &ac);
    return Vector_normalize(&normal);
}

Surface *Surface_new(){
    Surface *surface = malloc(sizeof(Surface));
    surface->type = GENERIC;
    surface->numTriangles = 0;
    surface->triangles = NULL;
    surface->center = NULL;
    surface->color = Color_new(0x000000);
    surface->maxDistanceFromCenter = 0;
    surface->reflexivity = 0;
    surface->smoothness = 0.5;
    return surface;
}

Surface *Surface_createSphere(Point *center, double radius, double reflexivity, double smoothness, Color color){
    int numPoints = (LAT_DIVS + 1) * LON_DIVS;
    Point **points = malloc(numPoints * sizeof(Point*));
    Surface *sphere = Surface_new();

    int index = 0;
    for (int i = 0; i <= LAT_DIVS; i++) {
        double theta = M_PI * i / LAT_DIVS; // polar angle from 0 to PI
        for (int j = 0; j < LON_DIVS; j++) {
            double phi = 2 * M_PI * j / LON_DIVS; // azimuth from 0 to 2PI

            double x = center->x + radius * sin(theta) * cos(phi);
            double y = center->y + radius * sin(theta) * sin(phi);
            double z = center->z + radius * cos(theta);

            points[index++] = Point_init(x, y, z);
        }
    }

    // Create triangles
    int tri_count = LAT_DIVS * LON_DIVS * 2;
    sphere->triangles = malloc(tri_count * sizeof(Triangle*));
    sphere->numTriangles = tri_count;
    int t = 0;
    for (int i = 0; i < LAT_DIVS; i++) {
        for (int j = 0; j < LON_DIVS; j++) {
            int curr = i * LON_DIVS + j;
            int next = curr + LON_DIVS;

            int right = (j + 1) % LON_DIVS;
            int curr_right = i * LON_DIVS + right;
            int next_right = (i + 1) * LON_DIVS + right;

            // Triangle 1
            sphere->triangles[t++] = Triangle_init(
                points[curr],
                points[next],
                points[next_right]
            );

            // Triangle 2
            sphere->triangles[t++] = Triangle_init(
                points[curr],
                points[next_right],
                points[curr_right]
            );
        }
    }
    sphere->maxDistanceFromCenter = radius;
    sphere->center = center;
    sphere->color = color;
    sphere->type = SPHERE;
    sphere->reflexivity = reflexivity;
    sphere->smoothness = smoothness;
    return sphere;
}

double Surface_area(Surface *surface){
    double area = 0;
    for(int i = 0; i < surface->numTriangles; i++){
        area += Triangle_area(surface->triangles[i]);
    }
    return area;
}

Surface *Surface_createRectXY(Point *origin, double width, double height, double reflexivity, double smoothness, Color color){
    Surface *rect = Surface_new();
    if(rect == NULL) return NULL;

    double x, y, z;
    x = origin->x;
    y = origin->y;
    z = origin->z;

    rect->center = Point_init(x + width/2, y + height/2, z);
    rect->maxDistanceFromCenter = sqrt(pow(width/2, 2) + pow(height/2, 2));

    Point *p1, *p2, *p3;

    p1 = Point_init(x + width, y, z);
    p2 = Point_init(x, y + height, z);
    p3 = Point_init(x + width, y + height, z);


    rect->numTriangles = 2;

    rect->triangles = malloc(rect->numTriangles *sizeof(Triangle*));
    rect->triangles[0] = Triangle_init(origin, p1, p2);
    rect->triangles[1] = Triangle_init(p1, p2, p3);

    rect->color = color;
    rect->reflexivity = reflexivity;
    rect->smoothness = smoothness;
    return rect;
}

Surface *Surface_createRectXZ(Point *origin, double width, double height, double reflexivity, double smoothness, Color color){
    Surface *rect = Surface_new();
    if(rect == NULL) return NULL;

    double x, y, z;
    x = origin->x;
    y = origin->y;
    z = origin->z;

    rect->center = Point_init(x + width/2, y, z + height/2);
    rect->maxDistanceFromCenter = sqrt(pow(width/2, 2) + pow(height/2, 2));

    Point *p1, *p2, *p3;

    p1 = Point_init(x + width, y, z);
    p2 = Point_init(x, y, z + height);
    p3 = Point_init(x + width, y, z + height);


    rect->numTriangles = 2;

    rect->triangles = malloc(rect->numTriangles *sizeof(Triangle*));
    rect->triangles[0] = Triangle_init(origin, p2, p1);
    rect->triangles[1] = Triangle_init(p1, p2, p3);

    rect->color = color;
    rect->reflexivity = reflexivity;
    rect->smoothness = smoothness;
    return rect;
}

Surface *Surface_createRectYZ(Point *origin, double width, double height, double reflexivity, double smoothness, Color color){
    Surface *rect = Surface_new();
    if(rect == NULL) return NULL;

    double x, y, z;
    x = origin->x;
    y = origin->y;
    z = origin->z;

    rect->center = Point_init(x, y + height/2, z + width/2);
    rect->maxDistanceFromCenter = sqrt(pow(width/2, 2) + pow(height/2, 2));

    Point *p1, *p2, *p3;

    p1 = Point_init(x, y, z + width);
    p2 = Point_init(x, y + height, z);
    p3 = Point_init(x, y + height, z + width);


    rect->numTriangles = 2;

    rect->triangles = malloc(rect->numTriangles *sizeof(Triangle*));
    rect->triangles[0] = Triangle_init(origin, p1, p2);
    rect->triangles[1] = Triangle_init(p1, p2, p3);

    rect->color = color;
    rect->reflexivity = reflexivity;
    rect->smoothness = smoothness;
    return rect;
}

Surface *Surface_createBox(Point *origin, double width, double height, double depth, double reflexivity, double smoothness, Color color) {
    Surface *box = Surface_new();
    if (box == NULL) return NULL;

    double x = origin->x;
    double y = origin->y;
    double z = origin->z;

    Point *points[8] = {
        Point_init(x, y, z),                               // 0: origin
        Point_init(x + width, y, z),                       // 1
        Point_init(x, y + height, z),                      // 2
        Point_init(x + width, y + height, z),              // 3
        Point_init(x, y, z + depth),                       // 4
        Point_init(x + width, y, z + depth),               // 5
        Point_init(x, y + height, z + depth),              // 6
        Point_init(x + width, y + height, z + depth)       // 7
    };

    box->center = Point_init(x + width / 2, y + height / 2, z + depth / 2);
    box->maxDistanceFromCenter = sqrt(width*width + height*height + depth*depth) / 2;

    // Triangle indices for each face (2 triangles per face)
    int faces[12][3] = {
        {0, 1, 2}, {1, 2, 3},     // Front
        {4, 5, 6}, {5, 6, 7},     // Back
        {0, 2, 4}, {2, 4, 6},     // Left
        {1, 5, 7}, {1, 3, 7},     // Right
        {0, 1, 4}, {5, 4, 1},     // Bottom
        {2, 3, 6}, {3, 6, 7}      // Top
    };

    box->numTriangles = 12;
    box->triangles = malloc(box->numTriangles * sizeof(Triangle *));
    if (!box->triangles) return NULL;

    for (int i = 0; i < 12; ++i) {
        int *f = faces[i];
        box->triangles[i] = Triangle_init(points[f[0]], points[f[1]], points[f[2]]);
    }

    box->color = color;
    box->reflexivity = reflexivity;
    box->smoothness = smoothness;
    return box;
}
