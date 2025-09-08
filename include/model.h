#ifndef MODEL_H
#define MODEL_H

#include<stdint.h>
#include"geometry.h"
#include"color.h"
#include"triangle.h"

#define LAT_DIVS 20
#define LON_DIVS 20

/**
 * Represents the material properties of a 3D model.
 */
typedef struct{
	/** Ambient light intensity */
	float ambient;
	/** Diffuse light color */
	Color diffuse;
	/** Specular light color */
	Color specular;
	/** Specular exponent (shininess) */
	int specularExponent;
	/** Model reflection coefficient (0 = no reflection, 1 = perfect mirror). */
	float reflexivity;
}Material;

Material Material_new(Color diffuse, float ambient, Color specular, int specularExponent, float reflexivity);


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
	float boundingRadius;
	/** Type of model. */
	ModelType type;
}Model;


/**
 * Creates a rectangular model aligned to the X-Y plane.
 *
 * @param origin Pointer to the bottom-left corner of the rectangle.
 * @param width Length of the rectangle along the X-axis.
 * @param height Length of the rectangle along the Y-axis.
 * @param material Material properties of the rectangle.
 * 
 * @return Pointer to the newly created Model representing the rectangle,
 *         or NULL if memory allocation fails.
 */
Model *Model_createRectXY(Point *origin, float width, float height, Material material);

/**
 * Creates a rectangular model aligned to the X-Z plane.
 * 
 * @param origin Pointer to the bottom-left corner of the rectangle.
 * @param width Length of the rectangle along the X-axis.
 * @param height Length of the rectangle along the Z-axis.
 * @param material Material properties of the rectangle.
 * 
 * @return Pointer to the allocated Model representing the rectangle,
 *         or NULL if memory allocation fails.
 */
Model *Model_createRectXZ(Point *origin, float width, float height, Material material);

/**
 * Creates a rectangular model aligned to the Y-Z plane.
 * 
 * @param origin Pointer to the bottom-left corner of the rectangle.
 * @param width Length of the rectangle along the Z-axis.
 * @param height Length of the rectangle along the Y-axis.
 * @param material Material properties of the rectangle.
 * 
 * @return Pointer to the allocated Model representing the rectangle,
 *         or NULL if memory allocation fails.
 */
Model *Model_createRectYZ(Point *origin, float width, float height, Material material);

/**
 * Creates a 3D box-shaped Model object.
 * 
 * @param origin Pointer to the Point representing the minimum corner of the box.
 * @param width Length of the box along the X-axis.
 * @param height Length of the box along the Y-axis.
 * @param depth Length of the box along the Z-axis.
 * @param material Material properties of the box.
 * 
 * @return Pointer to the allocated Model representing the box 
 *         or NULL if allocation fails.
 */
Model *Model_createBox(Point *origin, float width, float height, float depth, Material material);

/**
 * Creates a spherical model centered at the given point.
 * 
 * @param center Pointer to the center point of the sphere.
 * @param radius The radius of the sphere.
 * @param material Material properties of the sphere.
 * 
 * @return Pointer to the allocated Model representing the sphere 
 *         or NULL if allocation fails.
 */
Model *Model_createSphere(Point *center, float radius, Material material);

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



size_t Material_size(Material material);

size_t Model_size(Model *model);

#endif //MODEL_H