#ifndef FILE_H
#define FILE_H

#include "const.h" 

typedef struct FNode FNode;
typedef struct FNode {
    FNode *next;
    FNode *prev;
    char fname[DIR_SIZE]; // Może zawierać nazwy nadfolderów
    char fmode[FMODE_LEN];
} FNode;

void permissions(char *file, char modeval[FMODE_LEN]);

#endif