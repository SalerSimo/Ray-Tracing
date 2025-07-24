#ifndef SURFACE_H
#define SURFACE_H

#include<stdint.h>
#include"geometry.h"
#include"color.h"

#define LAT_DIVS 20
#define LON_DIVS 20

typedef struct{
    Point *a, *b, *c;
}Triangle;

typedef enum{
    GENERIC, SPHERE
}SurfaceType;

/**
 * Represents a 3D surface composed of one or more triangles.
 *
 * A Surface is a collection of triangles with shared material properties such as
 * color, reflectivity, and smoothness. It also includes spatial metadata like the
 * center point and a bounding radius for optimization (e.g., bounding sphere tests).
 */
typedef struct{
    /** Number of triangles of the surface. */
    int numTriangles;
    /** Array of pointers to Triangle structure. */
    Triangle **triangles;
    /** Center of the surface. */
    Point *center;
    /** Color of the surface. */
    Color color;
    /** Maximum distance from the center to any point on the surface (bounding radius). */
    double maxDistanceFromCenter;
    /** Surface reflection coefficient (0 = no reflection, 1 = perfect mirror). */
    double reflexivity;
    /** Surface smoothness (0 = fully rough, 1 = fully smooth). */
    double smoothness;
    /** Type of surface. */
    SurfaceType type;
}Surface;


/**
 * Creates a rectangular surface aligned to the X-Y plane.
 *
 * @param origin Pointer to the bottom-left corner of the rectangle.
 * @param width Length of the rectangle along the X-axis.
 * @param height Length of the rectangle along the Y-axis.
 * @param reflexivity Surface reflection coefficient (0 = no reflection, 1 = perfect mirror).
 * @param smoothness Surface smoothness (0 = fully rough, 1 = fully smooth).
 * @param color 32-bit RGB color of the box surface.
 * 
 * @return Pointer to the newly created Surface representing the rectangle,
 *         or NULL if memory allocation fails.
 */
Surface *Surface_createRectXY(Point *origin, double width, double height, double reflexivity, double smoothness, Color color);

/**
 * Creates a rectangular surface aligned to the X-Z plane.
 * 
 * @param origin Pointer to the bottom-left corner of the rectangle.
 * @param width Length of the rectangle along the X-axis.
 * @param height Length of the rectangle along the Z-axis.
 * @param reflexivity Surface reflection coefficient (0 = no reflection, 1 = perfect mirror).
 * @param smoothness Surface smoothness (0 = fully rough, 1 = fully smooth).
 * @param color 32-bit RGB color of the box surface.
 * 
 * @return Pointer to the allocated Surface representing the rectangle,
 *         or NULL if memory allocation fails.
 */
Surface *Surface_createRectXZ(Point *origin, double width, double height, double reflexivity, double smoothness, Color color);

/**
 * Creates a rectangular surface aligned to the Y-Z plane.
 * 
 * @param origin Pointer to the bottom-left corner of the rectangle.
 * @param width Length of the rectangle along the Z-axis.
 * @param height Length of the rectangle along the Y-axis.
 * @param reflexivity Surface reflection coefficient (0 = no reflection, 1 = perfect mirror).
 * @param smoothness Surface smoothness (0 = fully rough, 1 = fully smooth).
 * @param color 32-bit RGB color of the box surface.
 * 
 * @return Pointer to the allocated Surface representing the rectangle,
 *         or NULL if memory allocation fails.
 */
Surface *Surface_createRectYZ(Point *origin, double width, double height, double reflexivity, double smoothness, Color color);

/**
 * Creates a 3D box-shaped Surface object.
 * 
 * @param origin Pointer to the Point representing the minimum corner of the box.
 * @param width Length of the box along the X-axis.
 * @param height Length of the box along the Y-axis.
 * @param depth Length of the box along the Z-axis.
 * @param reflexivity Surface reflection coefficient (0 = no reflection, 1 = perfect mirror).
 * @param smoothness Surface smoothness (0 = fully rough, 1 = fully smooth).
 * @param color 32-bit RGB color of the box surface.
 * 
 * @return Pointer to the allocated Surface representing the box 
 *         or NULL if allocation fails.
 */
Surface *Surface_createBox(Point *origin, double width, double height, double depth, double reflexivity, double smoothness, Color color);

/**
 * Creates a spherical surface centered at the given point.
 * 
 * @param conter Pointer to the center point of the sphere.
 * @param radius The radius of the sphere
 * @param reflexivity Surface reflection coefficient (0 = no reflection, 1 = perfect mirror).
 * @param smoothness Surface smoothness (0 = fully rough, 1 = fully smooth).
 * @param color 32-bit RGB color of the box surface.
 * 
 * @return Pointer to the allocated Surface representing the sphere 
 *         or NULL if allocation fails.
 */
Surface *Surface_createSphere(Point *center, double radius, double reflexivity, double smoothness, Color color);

/**
 * Computes and returns the surface normal of a triangle.
 * The normal is calculated using the cross product of two edges of the triangle,
 * and is normalized to 1 unit length.
 *
 * @param t Pointer to the Triangle.
 * @return The normal Vector of the triangle.
 */
Vector Triangle_getNormal(Triangle *t);

#endif //SURFACE_H