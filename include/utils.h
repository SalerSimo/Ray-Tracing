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

/**
 * @brief Extracts the directory portion of a file path.
 *
 * Given a full file path (e.g., "/path/to/your/file/myFile.txt"),
 * this function returns the directory path (e.g., "/path/to/your/file/").
 * If the path contains no directory separators, "./" is returned to indicate the current directory.
 *
 * @param fullPath The full file path.
 * @return A string containing the directory path.
 */
char *GetDirectoryPath(char *fullPath);


#endif //UTILS_H