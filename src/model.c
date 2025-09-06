#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<stdint.h>
#include"model.h"

Triangle *Triangle_init(Point *a, Point *b, Point *c, unsigned char material){
	Triangle *t = malloc(sizeof(Triangle));
	t->a = a;
	t->b = b;
	t->c = c;
	t->material = material;
	return t;
}

double Triangle_area(Triangle *t){
	Vector ab, ac;
	ab = Vector_fromPoints(t->a, t->b);
	ac = Vector_fromPoints(t->a, t->c);
	Vector v = Vector_crossProduct(ab, ac);
	return sqrt(v.normSquared) / 2;
}

Vector Triangle_getNormal(Triangle *t){
	Vector ab, ac;
	ab = Vector_fromPoints(t->a, t->b);
	ac = Vector_fromPoints(t->a, t->c);
	Vector normal = Vector_crossProduct(ab, ac);
	return Vector_normalize(normal);
}

Model *Model_new(){
	Model *model = malloc(sizeof(Model));
	model->type = GENERIC;
	model->numTriangles = 0;
	model->triangles = NULL;
	model->center = NULL;
	model->maxDistanceFromCenter = 0;
	return model;
}

Model *Model_createSphere(Point *center, double radius, double reflexivity, double shininess, Color color){
	int numPoints = (LAT_DIVS + 1) * LON_DIVS;
	Point **points = malloc(numPoints * sizeof(Point*));
	Model *sphere = Model_new();

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
				points[next_right],
				0
			);

			// Triangle 2
			sphere->triangles[t++] = Triangle_init(
				points[curr],
				points[next_right],
				points[curr_right],
				0
			);
		}
	}
	sphere->maxDistanceFromCenter = radius;
	sphere->center = center;
	sphere->type = SPHERE;

	sphere->materials = malloc(sizeof(Material));
	sphere->numMaterials = 1;
	sphere->materials[0].color = color;
	sphere->materials[0].reflexivity = reflexivity;
	sphere->materials[0].shininess = shininess;
	return sphere;
}

double Model_area(Model *model){
	double area = 0;
	for(int i = 0; i < model->numTriangles; i++){
		area += Triangle_area(model->triangles[i]);
	}
	return area;
}

Model *Model_createRectXY(Point *origin, double width, double height, double reflexivity, double shininess, Color color){
	Model *rect = Model_new();
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
	rect->triangles[0] = Triangle_init(origin, p1, p2, 0);
	rect->triangles[1] = Triangle_init(p1, p2, p3, 0);

	rect->materials = malloc(sizeof(Material));
	rect->numMaterials = 1;
	rect->materials[0].color = color;
	rect->materials[0].reflexivity = reflexivity;
	rect->materials[0].shininess = shininess;
	return rect;
}

Model *Model_createRectXZ(Point *origin, double width, double height, double reflexivity, double shininess, Color color){
	Model *rect = Model_new();
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
	rect->triangles[0] = Triangle_init(origin, p2, p1, 0);
	rect->triangles[1] = Triangle_init(p1, p2, p3, 0);

	rect->materials = malloc(sizeof(Material));
	rect->numMaterials = 1;
	rect->materials[0].color = color;
	rect->materials[0].reflexivity = reflexivity;
	rect->materials[0].shininess = shininess;
	return rect;
}

Model *Model_createRectYZ(Point *origin, double width, double height, double reflexivity, double shininess, Color color){
	Model *rect = Model_new();
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
	rect->triangles[0] = Triangle_init(origin, p1, p2, 0);
	rect->triangles[1] = Triangle_init(p1, p2, p3, 0);

	rect->materials = malloc(sizeof(Material));
	rect->numMaterials = 1;
	rect->materials[0].color = color;
	rect->materials[0].reflexivity = reflexivity;
	rect->materials[0].shininess = shininess;
	return rect;
}

Model *Model_createBox(Point *origin, double width, double height, double depth, double reflexivity, double shininess, Color color) {
	Model *box = Model_new();
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
		box->triangles[i] = Triangle_init(points[f[0]], points[f[1]], points[f[2]], 0);
	}


	box->materials = malloc(sizeof(Material));
	box->numMaterials = 1;
	box->materials[0].color = color;
	box->materials[0].reflexivity = reflexivity;
	box->materials[0].shininess = shininess;
	return box;
}

void Triangle_translate(Triangle *t, Vector translation){
	t->a = Point_translate(t->a, translation);
	t->b = Point_translate(t->b, translation);
	t->c = Point_translate(t->c, translation);
}

void Model_translate(Model *model, Vector translation){
	if(model == NULL) return;
	model->center = Point_translate(model->center, translation);
	for(int i = 0; i < model->numTriangles; i++){
		Triangle_translate(model->triangles[i], translation);
	}
}

void Model_scale(Model *model, double scalar){
	if(model == NULL || scalar < 0) return;
	model->maxDistanceFromCenter *= scalar;
	for(int i = 0; i < model->numTriangles; i++){
		Vector v;
		Triangle *t = model->triangles[i];

		v = Vector_fromPoints(model->center, t->a);
		v = Vector_scale(v, scalar);
		t->a->x = model->center->x + v.x;
		t->a->y = model->center->y + v.y;
		t->a->z = model->center->z + v.z;

		v = Vector_fromPoints(model->center, t->b);
		v = Vector_scale(v, scalar);
		t->b->x = model->center->x + v.x;
		t->b->y = model->center->y + v.y;
		t->b->z = model->center->z + v.z;

		v = Vector_fromPoints(model->center, t->c);
		v = Vector_scale(v, scalar);
		t->c->x = model->center->x + v.x;
		t->c->y = model->center->y + v.y;
		t->c->z = model->center->z + v.z;
	}
}

Point *Triangle_center(Triangle *t){
	Point *center = Point_init(0, 0, 0);
	center->x = (t->a->x + t->b->x + t->c->x) / 3;
	center->y = (t->a->y + t->b->y + t->c->y) / 3;
	center->z = (t->a->z + t->b->z + t->c->z) / 3;

	return center;
}

static Point *g_refPoint;

int compareTriangles(const void *a, const void *b) {
	Triangle *t1 = *(Triangle **)a;
	Triangle *t2 = *(Triangle **)b;

	double d1 = Point_distanceSquared(g_refPoint, Triangle_center(t1));
	double d2 = Point_distanceSquared(g_refPoint, Triangle_center(t2));

	if (d1 < d2) return -1;
	else if (d1 > d2) return 1;
	else return 0;
}

void Model_sortTriangles(Model *model, Point *point){
	if(model == NULL || point == NULL) return;
	if(model->triangles == NULL) return;
	g_refPoint = point;
	qsort(model->triangles, model->numTriangles, sizeof(Triangle*), compareTriangles);
}