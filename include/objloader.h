#ifndef OBJLOADER_H
#define OBJLOADER_H

#include"surface.h"
#include"utils.h"
#include<stdio.h>

/**
 * @brief Write a Surface to a .obj file.
 * 
 * @param surface Pointer to the surface to export.
 * @param fileName Path of the output .obj file.
 * 
 * @return 0 in case of success, -1 if the file could not be opened.
 */
int Surface_toOBJ(const Surface *surface, const char *fileName);

/**
 * @brief Parses a Wavefront .obj file and creates a Surface from it.
 * 
 * This function reads geometry data from the specified .obj file
 * and constructs a Surface object. All faces in the file must be triangles.
 * 
 * @param fileName Path to the .obj file to load, relative to the project directory.
 * 
 * @return Pointer to a Surface object on success,
 *         or NULL if the file cannot be read or is invalid.
 */
Surface* Surface_fromOBJ(const char *fileName);


#endif //OBJLOADER_H