#ifndef MODEL_H
#define MODEL_H

#include<stdint.h>
#include"geometry.h"
#include"color.h"

#define LAT_DIVS 20
#define LON_DIVS 20

typedef struct{
	Color color;
	double reflexivity;
	double shininess;
}Material;

typedef struct{
	Point *a, *b, *c;
	Color color;
}Triangle;

typedef enum{
	GENERIC, SPHERE, LIGHT
}ModelType;

/**
 * Represents a 3D model composed of one or more triangles.
 *
 * A Model is a collection of triangles with shared material properties such as
 * color, reflectivity, and smoothness. It also includes spatial metadata like the
 * center point and a bounding radius for optimization (e.g., bounding sphere tests).
 */
typedef struct{
	/** Number of triangles of the model. */
	int numTriangles;
	/** Array of pointers to Triangle structure. */
	Triangle **triangles;
	/** Center of the model. */
	Point *center;
	/** Color of the model. */
	Color color;
	/** Maximum distance from the center to any point on the model (bounding radius). */
	double maxDistanceFromCenter;
	/** Model reflection coefficient (0 = no reflection, 1 = perfect mirror). */
	double reflexivity;
	/** Model shininess (0 = fully rough, 1 = fully smooth). */
	double shininess;
	/** Type of model. */
	ModelType type;
}Model;


/**
 * Creates a rectangular model aligned to the X-Y plane.
 *
 * @param origin Pointer to the bottom-left corner of the rectangle.
 * @param width Length of the rectangle along the X-axis.
 * @param height Length of the rectangle along the Y-axis.
 * @param reflexivity Model reflection coefficient (0 = no reflection, 1 = perfect mirror).
 * @param shininess Model shininess (0 = fully rough, 1 = fully smooth).
 * @param color 32-bit RGB color of the box model.
 * 
 * @return Pointer to the newly created Model representing the rectangle,
 *         or NULL if memory allocation fails.
 */
Model *Model_createRectXY(Point *origin, double width, double height, double reflexivity, double shininess, Color color);

/**
 * Creates a rectangular model aligned to the X-Z plane.
 * 
 * @param origin Pointer to the bottom-left corner of the rectangle.
 * @param width Length of the rectangle along the X-axis.
 * @param height Length of the rectangle along the Z-axis.
 * @param reflexivity Model reflection coefficient (0 = no reflection, 1 = perfect mirror).
 * @param shininess Model shininess (0 = fully rough, 1 = fully smooth).
 * @param color 32-bit RGB color of the box model.
 * 
 * @return Pointer to the allocated Model representing the rectangle,
 *         or NULL if memory allocation fails.
 */
Model *Model_createRectXZ(Point *origin, double width, double height, double reflexivity, double shininess, Color color);

/**
 * Creates a rectangular model aligned to the Y-Z plane.
 * 
 * @param origin Pointer to the bottom-left corner of the rectangle.
 * @param width Length of the rectangle along the Z-axis.
 * @param height Length of the rectangle along the Y-axis.
 * @param reflexivity Model reflection coefficient (0 = no reflection, 1 = perfect mirror).
 * @param shininess Model shininess (0 = fully rough, 1 = fully smooth).
 * @param color 32-bit RGB color of the box model.
 * 
 * @return Pointer to the allocated Model representing the rectangle,
 *         or NULL if memory allocation fails.
 */
Model *Model_createRectYZ(Point *origin, double width, double height, double reflexivity, double shininess, Color color);

/**
 * Creates a 3D box-shaped Model object.
 * 
 * @param origin Pointer to the Point representing the minimum corner of the box.
 * @param width Length of the box along the X-axis.
 * @param height Length of the box along the Y-axis.
 * @param depth Length of the box along the Z-axis.
 * @param reflexivity Model reflection coefficient (0 = no reflection, 1 = perfect mirror).
 * @param shininess Model shininess (0 = fully rough, 1 = fully smooth).
 * @param color 32-bit RGB color of the box model.
 * 
 * @return Pointer to the allocated Model representing the box 
 *         or NULL if allocation fails.
 */
Model *Model_createBox(Point *origin, double width, double height, double depth, double reflexivity, double shininess, Color color);

/**
 * Creates a spherical model centered at the given point.
 * 
 * @param conter Pointer to the center point of the sphere.
 * @param radius The radius of the sphere
 * @param reflexivity Model reflection coefficient (0 = no reflection, 1 = perfect mirror).
 * @param smoothness Model smoothness (0 = fully rough, 1 = fully smooth).
 * @param color 32-bit RGB color of the box model.
 * 
 * @return Pointer to the allocated Model representing the sphere 
 *         or NULL if allocation fails.
 */
Model *Model_createSphere(Point *center, double radius, double reflexivity, double shininess, Color color);

/**
 * @brief Translates all vertices of the Model by a given vector.
 * 
 * @param model Pointer to the Model to translate.
 * @param translation Pointer to the Vector representing the translation offset.
 */
void Model_translate(Model *model, Vector translation);

/**
 * @brief Scales a model by a given scalar.
 * 
 * @param model Pointer to the model to scale.
 * @param scalar The factor to scale the model with.
 */
void Model_scale(Model *model, double scalar);

/**
 * @brief Sorts the triangles of a model by their distance to a reference point.
 *
 * @param model Pointer to the model whose triangles will be sorted.
 * @param point Reference point used to calculate distances.
 */
void Model_sortTriangles(Model *model, Point *point);

/**
 * Computes and returns the model normal of a triangle.
 * The normal is calculated using the cross product of two edges of the triangle,
 * and is normalized to 1 unit length.
 *
 * @param t Pointer to the Triangle.
 * @return The normal Vector of the triangle.
 */
Vector Triangle_getNormal(Triangle *t);

#endif //MODEL_H