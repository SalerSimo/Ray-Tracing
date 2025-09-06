#ifndef MODEL_H
#define MODEL_H

#include<stdint.h>
#include"geometry.h"
#include"color.h"

#define LAT_DIVS 20
#define LON_DIVS 20

/**
 * Represents the material properties of a 3D model.
 */
typedef struct{
	/** Color of the material. */
	Color color;
	/** Model reflection coefficient (0 = no reflection, 1 = perfect mirror). */
	float reflexivity;
	/** Model shininess (0 = fully rough, 1 = fully smooth). */
	float shininess;
}Material;

typedef struct{
	Point *a, *b, *c;
	/** Index of the material */
	unsigned char material;
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
	/** Array of materials used by the model. */
	Material *materials;
	/** Number of materials used by the model. */
	int numMaterials;
	/** Number of triangles of the model. */
	int numTriangles;
	/** Array of pointers to Triangle structure. */
	Triangle **triangles;
	/** Center of the model. */
	Point *center;
	/** Maximum distance from the center to any point on the model (bounding radius). */
	float maxDistanceFromCenter;
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
Model *Model_createRectXY(Point *origin, float width, float height, float reflexivity, float shininess, Color color);

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
Model *Model_createRectXZ(Point *origin, float width, float height, float reflexivity, float shininess, Color color);

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
Model *Model_createRectYZ(Point *origin, float width, float height, float reflexivity, float shininess, Color color);

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
Model *Model_createBox(Point *origin, float width, float height, float depth, float reflexivity, float shininess, Color color);

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
Model *Model_createSphere(Point *center, float radius, float reflexivity, float shininess, Color color);

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
void Model_scale(Model *model, float scalar);

/**
 * @brief Sorts the triangles of a model by their distance to a reference point.
 *
 * @param model Pointer to the model whose triangles will be sorted.
 * @param point Reference point used to calculate distances.
 */
void Model_sortTriangles(Model *model, Point *point);

/**
 * @brief Allocates and initializes a new Triangle structure.
 * @param a Pointer to the first vertex of the triangle.
 * @param b Pointer to the second vertex of the triangle.
 * @param c Pointer to the third vertex of the triangle.
 * @param material Material identifier for the triangle.
 * @return Pointer to the newly created Triangle structure, or NULL if allocation fails.
 */
Triangle *Triangle_init(Point *a, Point *b, Point *c, unsigned char material);

/**
 * Computes and returns the model normal of a triangle.
 * The normal is calculated using the cross product of two edges of the triangle,
 * and is normalized to 1 unit length.
 *
 * @param t Pointer to the Triangle.
 * @return The normal Vector of the triangle.
 */
Vector Triangle_getNormal(Triangle *t);

size_t Triangle_size(Triangle *t);

size_t Material_size(Material material);

size_t Model_size(Model *model);

#endif //MODEL_H