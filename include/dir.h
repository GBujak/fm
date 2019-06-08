#ifndef DIR_H
#define DIR_H

#include <dirent.h>
#include <stdbool.h>

#include "const.h"
#include "file.h"

typedef struct FList {
    struct FNode *start;
    struct FNode *end;
} FList;

typedef struct QNode QNode;
typedef struct QNode {
    struct FNode *n;
    QNode *next;
    QNode *prev;
    bool selected;
} QNode;

typedef struct QList {
    QNode *start;
    QNode *end;
} QList;

void append(FList *fl, struct FNode *f);
void dump(FList *l);
bool listdir(FList *fl, int recursion, char* dir);
void print_flist(FList *fl);

QList fzf(FList *fl, const char* query);
void print_qlist(QList *fl);
void qappend(QList *fl, QNode *f);
void qdump(QList *l);
void qdump_unselected(QList *l);

#endif