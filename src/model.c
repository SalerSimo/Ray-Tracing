#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<stdint.h>
#include"model.h"


Model *Model_new(){
	Model *model = malloc(sizeof(Model));
	if(model == NULL){
		printf("ERROR::MODEL::Model_new::Failed to allocate memory for Model\n");
		return NULL;
	}
	model->type = GENERIC;
	model->numTriangles = 0;
	model->triangles = NULL;
	model->center = NULL;
	model->boundingRadius = 0;
	return model;
}

Model *Model_createSphere(Point *center, float radius, Material material){
	int numPoints = (LAT_DIVS + 1) * LON_DIVS;
	Point **points = malloc(numPoints * sizeof(Point*));
	if(points == NULL){
		printf("ERROR::MODEL::Model_createSphere::Failed to allocate memory for sphere points\n");
		return NULL;
	}
	Model *sphere = Model_new();

	int index = 0;
	for (int i = 0; i <= LAT_DIVS; i++) {
		float theta = M_PI * i / LAT_DIVS; // polar angle from 0 to PI
		for (int j = 0; j < LON_DIVS; j++) {
			float phi = 2 * M_PI * j / LON_DIVS; // azimuth from 0 to 2PI

			float x = center->x + radius * sin(theta) * cos(phi);
			float y = center->y + radius * sin(theta) * sin(phi);
			float z = center->z + radius * cos(theta);

			points[index++] = Point_init(x, y, z);
		}
	}

	// Create triangles
	int tri_count = LAT_DIVS * LON_DIVS * 2;
	sphere->triangles = malloc(tri_count * sizeof(Triangle*));
	if(sphere->triangles == NULL){
		printf("ERROR::MODEL::Model_createSphere::Failed to allocate memory for sphere triangles\n");
		return NULL;
	}
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
	sphere->boundingRadius = radius;
	sphere->center = center;
	sphere->type = SPHERE;

	sphere->materials = malloc(sizeof(Material));
	if(sphere->materials == NULL){
		printf("ERROR::MODEL::Model_createSphere::Failed to allocate memory for sphere material\n");
		return NULL;
	}
	sphere->numMaterials = 1;
	sphere->materials[0] = material;
	return sphere;
}

Model *Model_createRectXY(Point *origin, float width, float height, Material material){
	Model *rect = Model_new();
	if(rect == NULL) return NULL;

	float x, y, z;
	x = origin->x;
	y = origin->y;
	z = origin->z;

	rect->center = Point_init(x + width/2, y + height/2, z);
	rect->boundingRadius = sqrt(pow(width/2, 2) + pow(height/2, 2));

	Point *p1, *p2, *p3;

	p1 = Point_init(x + width, y, z);
	p2 = Point_init(x, y + height, z);
	p3 = Point_init(x + width, y + height, z);


	rect->numTriangles = 2;

	rect->triangles = malloc(rect->numTriangles *sizeof(Triangle*));
	if(rect->triangles == NULL){
		printf("ERROR::MODEL::Model_createRectXY::Failed to allocate memory for rectangle triangles\n");
		return NULL;
	}
	rect->triangles[0] = Triangle_init(origin, p1, p2, 0);
	rect->triangles[1] = Triangle_init(p1, p2, p3, 0);

	rect->materials = malloc(sizeof(Material));
	if(rect->materials == NULL){
		printf("ERROR::MODEL::Model_createRectXY::Failed to allocate memory for rectangle material\n");
		return NULL;
	}
	rect->numMaterials = 1;
	rect->materials[0] = material;
	return rect;
}

Model *Model_createRectXZ(Point *origin, float width, float height, Material material){
	Model *rect = Model_new();
	if(rect == NULL) return NULL;

	float x, y, z;
	x = origin->x;
	y = origin->y;
	z = origin->z;

	rect->center = Point_init(x + width/2, y, z + height/2);
	rect->boundingRadius = sqrt(pow(width/2, 2) + pow(height/2, 2));

	Point *p1, *p2, *p3;

	p1 = Point_init(x + width, y, z);
	p2 = Point_init(x, y, z + height);
	p3 = Point_init(x + width, y, z + height);


	rect->numTriangles = 2;

	rect->triangles = malloc(rect->numTriangles *sizeof(Triangle*));
	if(rect->triangles == NULL){
		printf("ERROR::MODEL::Model_createRectXZ::Failed to allocate memory for rectangle triangles\n");
		return NULL;
	}
	rect->triangles[0] = Triangle_init(origin, p2, p1, 0);
	rect->triangles[1] = Triangle_init(p1, p2, p3, 0);

	rect->materials = malloc(sizeof(Material));
	if(rect->materials == NULL){
		printf("ERROR::MODEL::Model_createRectXZ::Failed to allocate memory for rectangle material\n");
		return NULL;
	}
	rect->numMaterials = 1;
	rect->materials[0] = material;
	return rect;
}

Model *Model_createRectYZ(Point *origin, float width, float height, Material material){
	Model *rect = Model_new();
	if(rect == NULL) return NULL;

	float x, y, z;
	x = origin->x;
	y = origin->y;
	z = origin->z;

	rect->center = Point_init(x, y + height/2, z + width/2);
	rect->boundingRadius = sqrt(pow(width/2, 2) + pow(height/2, 2));

	Point *p1, *p2, *p3;

	p1 = Point_init(x, y, z + width);
	p2 = Point_init(x, y + height, z);
	p3 = Point_init(x, y + height, z + width);


	rect->numTriangles = 2;

	rect->triangles = malloc(rect->numTriangles *sizeof(Triangle*));
	if(rect->triangles == NULL){
		printf("ERROR::MODEL::Model_createRectYZ::Failed to allocate memory for rectangle triangles\n");
		return NULL;
	}
	rect->triangles[0] = Triangle_init(origin, p1, p2, 0);
	rect->triangles[1] = Triangle_init(p1, p2, p3, 0);

	rect->materials = malloc(sizeof(Material));
	if(rect->materials == NULL){
		printf("ERROR::MODEL::Model_createRectYZ::Failed to allocate memory for rectangle material\n");
		return NULL;
	}
	rect->numMaterials = 1;
	rect->materials[0] = material;
	return rect;
}

Model *Model_createBox(Point *origin, float width, float height, float depth, Material material) {
	Model *box = Model_new();
	if (box == NULL) return NULL;

	float x = origin->x;
	float y = origin->y;
	float z = origin->z;

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
	box->boundingRadius = sqrt(width*width + height*height + depth*depth) / 2;

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
	if(box->triangles == NULL){
		printf("ERROR::MODEL::Model_createBox::Failed to allocate memory for box triangles\n");
		return NULL;
	}

	for (int i = 0; i < 12; ++i) {
		int *f = faces[i];
		box->triangles[i] = Triangle_init(points[f[0]], points[f[1]], points[f[2]], 0);
	}


	box->materials = malloc(sizeof(Material));
	if(box->materials == NULL){
		printf("ERROR::MODEL::Model_createBox::Failed to allocate memory for box material\n");
		return NULL;
	}
	box->numMaterials = 1;
	box->materials[0] = material;
	return box;
}


void Model_translate(Model *model, Vector translation){
	if(model == NULL) return;
	model->center = Point_translate(model->center, translation);
	for(int i = 0; i < model->numTriangles; i++){
		Triangle_translate(model->triangles[i], translation);
	}
}

void Model_scale(Model *model, float scalar){
	if(model == NULL || scalar < 0) return;
	model->boundingRadius *= scalar;
	for(int i = 0; i < model->numTriangles; i++){
		Vector v;
		Triangle *t = model->triangles[i];

		v = Vector_fromPoints(model->center, t->a);
		v = Vector_scale(v, scalar);
		free(t->a);
		t->a = Point_translate(model->center, v);

		v = Vector_fromPoints(model->center, t->b);
		v = Vector_scale(v, scalar);
		free(t->b);
		t->b = Point_translate(model->center, v);

		v = Vector_fromPoints(model->center, t->c);
		v = Vector_scale(v, scalar);
		free(t->c);
		t->c = Point_translate(model->center, v);
	}
}


static Point *g_refPoint;
int compareTriangles(const void *a, const void *b) {
	Triangle *t1 = *(Triangle **)a;
	Triangle *t2 = *(Triangle **)b;

	float d1 = Point_distanceSquared(g_refPoint, Triangle_center(t1));
	float d2 = Point_distanceSquared(g_refPoint, Triangle_center(t2));

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



size_t Material_size(Material material){
	return sizeof(material);
}

size_t Model_size(Model *model){
	size_t size = 0;

	size += sizeof(*model);
	size += model->numTriangles * sizeof(*model->triangles);
	size += Point_size(model->center);

	for(int i = 0; i < model->numMaterials; i++){
		size += Material_size(model->materials[i]);
	}
	for(int i = 0; i < model->numTriangles; i++){
		size += Triangle_size(model->triangles[i]);
	}

	return size;
}