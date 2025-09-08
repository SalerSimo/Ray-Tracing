#ifndef TRIANGLE_H
#define TRIANGLE_H

#include"geometry.h"
#include<stddef.h>

typedef struct{
	Point *a, *b, *c;
	/** Index of the material */
	unsigned char material;
}Triangle;

/**
 * @brief Allocates and initializes a new Triangle structure.
 * 
 * @param a Pointer to the first vertex of the triangle.
 * @param b Pointer to the second vertex of the triangle.
 * @param c Pointer to the third vertex of the triangle.
 * @param material Material identifier for the triangle.
 * @return Pointer to the newly created Triangle structure, or NULL if allocation fails.
 */
Triangle *Triangle_init(Point *a, Point *b, Point *c, unsigned char material);

/**
 * Computes and returns the normal vector of a triangle.
 * The normal is calculated using the cross product of two edges of the triangle,
 * and is normalized to 1 unit length.
 *
 * @param t Pointer to the Triangle.
 * @return The normal Vector of the triangle.
 */
Vector Triangle_getNormal(Triangle *t);

/**
 * @brief Translates the triangle by a given vector.
 * 
 * @param t Pointer to the Triangle to be translated.
 * @param translation The vector by which to translate the triangle.
 */
void Triangle_translate(Triangle *t, Vector translation);

/**
 * @brief Computes the centroid (center point) of the triangle.
 * 
 * @param t Pointer to the Triangle.
 * @return Pointer to a the Point representing the centroid of the triangle.
 */
Point *Triangle_center(Triangle *t);

/**
 * @brief Computes the memory size occupied by the Triangle structure and its components.
 * 
 * @param t Pointer to the Triangle.
 * 
 * @return Size in bytes of the Triangle structure and its components.
 */
size_t Triangle_size(Triangle *t);

#endif