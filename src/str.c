#include "../include/str.h"
#include "../include/env.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#define DIR_SIZE 4096
#define SUBSTITUTION_SIZE 20

char * filepath = "./folder/subfolder/subsubfolder/document.pdf";

void getpath (char* dest, const char* fname);
void getname (char* dest, const char* fname);
void getfullname (char* dest, const char* fname);
void getextension (char* dest, const char* fname);

struct substitution subs[] = {
    { "{/}", &getpath      },
    { "{|}", &getname      },
    { "{}",  &getfullname  },
    { "{.}", &getextension }
};

void getpath (char* dest, const char* fname) {
    int pathend;
    for (int i = 0; i < strlen(fname); i++)
        if (fname [i] == '/') pathend = i;
    int i;
    for (i = 0; i <= pathend; i++)
        dest[i] = fname[i];
    dest[i] = '\0';
}

void getname (char* dest, const char* fname) {
    int pathend;
    for (int i = 0; i < strlen(fname); i++)
        if (fname [i] == '/') pathend = i + 1;
    
    int nameend;
    for (int i = 0; i < strlen(fname); i++)
        if (fname [i] == '.') nameend = i;
    
    int i = 0;
    while (pathend != nameend) {
        dest[i++] = fname[pathend++];
    }
    dest[i] = '\0';
}

void getextension (char* dest, const char* fname) {
    int nameend;
    for (int i = 0; i < strlen(fname); i++)
        if (fname [i] == '.') nameend = i;
    int i = 0;
    while (fname[nameend] != '\0') {
        dest[i++] = fname[nameend++];
    }
    dest[i] = '\0';
}

void getfullname (char* dest, const char* fname) {
    int pathend = 0;
    //for (int i = 0; i < strlen(fname); i++)
    //    if (fname [i] == '/') pathend = i + 1;
    int i = 0;
    while (fname[pathend] != '\0') {
        dest[i++] = fname[pathend++];
    }
    dest[i] = '\0';
}

bool comparetoconst (const char* a, const char* _const) {
    if (strlen(a) < strlen(_const)) return false;
    for (int i = 0; i < strlen(_const); i++)
        if (a[i] != _const[i]) return false;
    return true;
}

int get_sub(const char* command, int index) {
    for (int j = 0; j < sizeof(subs) / sizeof(subs[0]); j++) {
        if (comparetoconst(&command[index], subs[j].key) == true) {
            return j;
        }
    }
    return -1;
}

void parsecommand (char* dest, const char* command, const char* fname) {
    char buffer[COMMAND_SIZE] = "\0";
    char* buff_ptr = buffer;
    for (int i = 0; i < strlen(command); i++) {
        int s = get_sub(command, i);
        if (s == -1) {
            *buff_ptr = command[i];
            buff_ptr++;
            *buff_ptr = '\0';
        } else {
            subs[s].func(buff_ptr, fname);
            i += strlen(subs[s].key) - 1;
            while (*buff_ptr != '\0') buff_ptr++;
        }
    }
    strcpy(dest, buffer);
}