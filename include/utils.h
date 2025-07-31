#ifndef UTILS_H
#define UTILS_H

/**
 * @brief Builds the full path to a file relative to the project directory.
 * 
 * @param fileName The relative file name (e.g., "src/data.txt").
 * 
 * @return A string containing the full path (e.g., "/path/to/project/src/data.txt"),
 *         or NULL if memory allocation fails.
 */
char* GetFullPath(char *fileName);


#endif //UTILS_H