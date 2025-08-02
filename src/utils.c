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

char *GetDirectoryPath(char *fullPath){
    if(fullPath == NULL) return strdup("./");
    size_t len = strlen(fullPath);
    int last = -1;
    for(int i = 0; i < len; i++){
        if(fullPath[i] == '/'){
            last = i;
        }
    }

    if(last == -1) return strdup("./");

    char *directoryPath = malloc((last + 2) * sizeof(char));
    for(int i = 0; i < last + 1; i++){
        directoryPath[i] = fullPath[i];
    }
    directoryPath[last + 1] = '\0';
    return directoryPath;
}