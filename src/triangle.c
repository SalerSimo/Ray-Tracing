#include"triangle.h"
#include<stdio.h>
#include<stdlib.h>

Triangle *Triangle_init(Point *a, Point *b, Point *c, unsigned char material){
	Triangle *t = malloc(sizeof(Triangle));
	if(t == NULL){
		printf("ERROR::TRIANGLE::Triangle_init::Failed to allocate memory for Triangle\n");
		return NULL;
	}
	t->a = Point_copy(a);
	t->b = Point_copy(b);
	t->c = Point_copy(c);
	t->material = material;
	return t;
}

Vector Triangle_getNormal(Triangle *t){
      if(t == NULL){
            printf("ERROR::TRIANGLE::Triangle_getNormal::Triangle is NULL\n");
            return Vector_init(0, 0, 0);
      }
	Vector ab, ac;
	ab = Vector_fromPoints(t->a, t->b);
	ac = Vector_fromPoints(t->a, t->c);
	Vector normal = Vector_crossProduct(ab, ac);
	return Vector_normalize(normal);
}

void Triangle_translate(Triangle *t, Vector translation){
	t->a = Point_translate(t->a, translation);
	t->b = Point_translate(t->b, translation);
	t->c = Point_translate(t->c, translation);
}

Point *Triangle_center(Triangle *t){
	Point *center = Point_init(0, 0, 0);
	center->x = (t->a->x + t->b->x + t->c->x) / 3;
	center->y = (t->a->y + t->b->y + t->c->y) / 3;
	center->z = (t->a->z + t->b->z + t->c->z) / 3;

	return center;
}

size_t Triangle_size(Triangle *t){
	size_t size = sizeof(*t);
	size += Point_size(t->a);
	size += Point_size(t->b);
	size += Point_size(t->c);
	return size;
}