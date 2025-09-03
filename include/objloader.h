#ifndef OBJLOADER_H
#define OBJLOADER_H

#include"model.h"
#include"utils.h"
#include<stdio.h>

/**
 * @brief Write a Model to a .obj file.
 * 
 * @param model Pointer to the model to export.
 * @param fileName Path of the output .obj file.
 * 
 * @return 0 in case of success, -1 if the file could not be opened.
 */
int Model_toOBJ(const Model *model, const char *fileName);

/**
 * @brief Parses a Wavefront .obj file and creates a Model from it.
 * 
 * This function reads geometry data from the specified .obj file
 * and constructs a Model object. All faces in the file must be triangles.
 * 
 * @param fileName Path to the .obj file to load, relative to the project directory.
 * 
 * @return Pointer to a Model object on success,
 *         or NULL if the file cannot be read or is invalid.
 */
Model* Model_fromOBJ(const char *fileName);


#endif //OBJLOADER_H