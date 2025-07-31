#ifndef PROJECT_DIR
#define PROJECT_DIR "."
#endif

#include"utils.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

char* GetFullPath(char *fileName){
    int length = strlen(PROJECT_DIR) + strlen(fileName) + 2;
    char *fullPath = malloc(length * sizeof(char));
    
    snprintf(fullPath, length, "%s/%s", PROJECT_DIR, fileName);
    return fullPath;
}