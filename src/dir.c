#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h> // tolower()
#include "../include/const.h"
#include "../include/env.h"
#include "../include/dir.h"
#include "../include/file.h"

bool listdir(FList *fl, int recursion, char* dir_name) {
    DIR *dir = opendir(dir_name);
    if (dir == NULL) {
        //printf("%s -> ", dir_name);
        //printf(strerror(errno));
        //printf("\n");
        return false;
    }
    struct dirent* dent;
    FNode* fnode;

    while((dent = readdir(dir)) != NULL) {
        if (
               strcmp(dent->d_name, ".") == 0
            || strcmp(dent->d_name, "..") == 0
            || dent->d_name[0] == '.'
        ) continue;

        fnode = malloc(sizeof(FNode));
        if (fnode == NULL) {
            puts("malloc error in listdir");
            abort();
        }

        fnode->fname[0] = '\0'; // przed tym fname ma losową zawartość
        strcat(fnode->fname, dir_name);
        strcat(fnode->fname, "/"); 

        strcat(fnode->fname, dent->d_name);
        permissions(fnode->fname, fnode->fmode);
        fnode->next = NULL;

        //printf("appending %s\n", fnode->fname);
        append(fl, fnode);

        if (
               fnode->fmode[0] == 'd'
            && recursion < MAX_RECURSION 
        ) {
            //printf("entering ----> \"%s\"\n", fnode->fname);
            listdir(fl, recursion+1, fnode->fname);
        }
    }
    //printf("leaving <---- \"%s\"\n", dir_name);
    return true;
}

void dump(FList *l) {
    FNode *f = l->start;
    FNode *temp;
    while (f != NULL) {
        temp = f;
        f = f->next;
        free(temp);
    }
}

void qdump(QList *l) {
    QNode *f = l->start;
    QNode *temp;
    while (f != NULL) {
        temp = f;
        f = f->next;
        free(temp);
    }
}

void qdump_unselected(QList *l) {
    QNode *f = l->start;
    QNode *temp;
    while (f != NULL) {
        if (f->selected == true) {
            f = f->next;
            continue;
        }
        if (f == l->start)
            l->start = f->next;
        if (f == l->end)
            l->end = f->prev;
        
        if (f->prev != NULL)
            f->prev->next = f->next;
        if (f->next != NULL)
            f->next->prev = f->prev;
        
        temp = f;
        f = f->next;
        free(temp);
    }
}

void append(FList *fl, FNode *f) {
    f->next = NULL;
    f->prev = fl->end;
    if (fl->start == NULL) fl->start = f;
    else fl->end->next = f;
    fl->end = f;
}

void qappend(QList *fl, QNode *f) {
    f->next = NULL;
    f->prev = fl->end;
    if (fl->start == NULL) fl->start = f;
    else fl->end->next = f;
    fl->end = f;
}

void print_flist(FList *fl) {
    FNode *f;
    f = fl->start;
    if (f==NULL) {
        puts("empty list to print");
        abort();
    }
    do {
        printf("%s\t%s\n", f->fmode, f->fname);
    } while ((f = f->next) != NULL);
}

void print_qlist(QList *fl) {
    QNode *f;
    f = fl->start;
    if (f==NULL) {
        puts("nothing found");
    }
    while (f != NULL) {
        printf("%s\t%s\n", f->n->fmode, f->n->fname);
        f = f->next;
    }
}

QList fzf(FList *fl, const char* query) {
    QList qfl = QUERYLIST;
    FNode *node = fl->start;
    int qi; // query index
    while (1) {
        if (node == NULL) return qfl;
        qi = 0;
        for (int i = 0; i < strlen(node->fname); i++) {
            if (query[qi] == '\0') break;
            if (tolower(node->fname[i]) == tolower(query[qi])) qi++;
        }
        if (query[qi] == '\0') {
            QNode* qn = malloc(sizeof(QNode));
            qn->selected = false;
            qn->n = node;
            qappend(&qfl, qn);
        }
        node = node->next;
    }
}
