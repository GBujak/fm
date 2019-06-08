#ifndef ENV_H
#define ENV_H

#include <stdbool.h>
#include "const.h"
#include "dir.h"

WINDOW *MAINWIN, *BOTWIN, *TOPWIN;
int MAXLINES, MAXCOLS;

char CURDIR[DIR_SIZE];
char QUERY[DIR_SIZE];

QNode *CURSOR; // stores currently selected file
int CPOS;

bool MAIN_LOOP;

FList DIRLIST;
QList QUERYLIST;

#endif