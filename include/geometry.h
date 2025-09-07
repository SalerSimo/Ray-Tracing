#ifndef GEOMETRY_H
#define GEOMETRY_H

typedef struct{
	float x, y, z;
	float normSquared;
}Vector;

typedef struct{
	float x, y, z;
}Point;

typedef struct{
	Point *origin;
	Vector direction;
}Line;



// ───── POINT ─────

// Constructor
Point*  Point_init(float x, float y, float z);
Point*  Point_copy(Point *p);

// Operations
float  Point_distanceSquared(Point *a, Point *b);
Point*  Point_translate(Point *p, Vector v);

// Debug
void    Point_print(Point *p);
int     Point_size(Point *p);


// ───── VECTOR ─────

// Constructor
Vector  Vector_init(float x, float y, float z);
Vector  Vector_fromPoints(Point *a, Point *b);

// Operations
Vector  Vector_normalize(Vector v);
Vector  Vector_scale(Vector v, float scalar);
Vector  Vector_sum(Vector v1, Vector v2);
Vector  Vector_crossProduct(Vector a, Vector b);
float  Vector_dot(Vector v1, Vector v2);
Vector  Vector_perpendicular(Vector v);
Vector  Vector_rotate(Vector v, Vector axis, float angle);
int     Vector_equal(Vector v1, Vector v2);

// Debug
void    Vector_print(Vector *v);
int	  Vector_size(Vector v);


// ───── LINE ─────

// Constructor
Line*   Line_init(Point *origin, Vector direction);

// Operations
float  Line_Point_distance(Line *l, Point *p);
Point*  Line_projectionPoint(Line *l, Point *p);




#endif // GEOMETRY_H