#ifndef GEOMETRY_H
#define GEOMETRY_H

typedef struct{
	double x, y, z;
	double normSquared;
}Vector;

typedef struct{
	double x, y, z;
}Point;

typedef struct{
	Point *origin;
	Vector direction;
}Line;

typedef struct{
	double A, B, C, D;
}Plane;


// ───── POINT ─────

// Constructor
Point*  Point_init(double x, double y, double z);

// Operations
double  Point_distanceSquared(Point *a, Point *b);
Point*  Point_translate(Point *p, Vector v);

// Debug
void    Point_print(Point *p);
int     Point_size(Point *p);


// ───── VECTOR ─────

// Constructor
Vector  Vector_init(double x, double y, double z);
Vector  Vector_fromPoints(Point *a, Point *b);

// Operations
Vector  Vector_normalize(Vector v);
Vector  Vector_scale(Vector v, double scalar);
Vector  Vector_sum(Vector v1, Vector v2);
Vector  Vector_crossProduct(Vector a, Vector b);
double  Vector_dot(Vector v1, Vector v2);
Vector  Vector_perpendicular(Vector v);
Vector  Vector_rotate(Vector v, Vector axis, double angle);
int     Vector_equal(Vector v1, Vector v2);

// Debug
void    Vector_print(Vector *v);
int	  Vector_size(Vector v);


// ───── LINE ─────

// Constructor
Line*   Line_init(Point *origin, Vector direction);

// Operations
double  Line_Point_distance(Line *l, Point *p);
Point*  Line_projectionPoint(Line *l, Point *p);

// Debug
void    Line_print(Line *l);


// ───── PLANE ─────

// Constructor
Plane*  Plane_new(double A, double B, double C, double D);

// Operations
Vector  Plane_vectorNormal(Plane *plane);


#endif // GEOMETRY_H