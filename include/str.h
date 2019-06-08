#ifndef STR_H
#define STR_H

#include "env.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#define SUBSTITUTION_SIZE 20
#define COMMAND_SIZE 12288

void getpath (char* dest, const char* fname);
void getname (char* dest, const char* fname);
void getfullname (char* dest, const char* fname);
void getextension (char* dest, const char* fname);

bool comparetoconst (const char* a, const char* _const);
int get_sub (const char* command, int index);
void parsecommand (char* dest, const char* command, const char* fname);


struct substitution {
    char key[SUBSTITUTION_SIZE];
    void (*func)(char* dest, const char* fname);
};

#endif