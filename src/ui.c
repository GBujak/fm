#include "../include/ui.h" 
#include "../include/const.h"
#include "../include/env.h"
#include "../include/str.h"
#include <curses.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void input_window(char *msg, char *response, int len) {
    int height = 4;
    int width = MAXCOLS - 6;
    WINDOW *win = newwin(
        height, width,
        MAXLINES / 2 - height / 2,
        MAXCOLS / 2 - width / 2
    );
    box(win, 0, 0);
    mvwprintw(win, 1, 1, msg);
    wmove(win, 2, 1);
    echo();
    wgetnstr(win, response, len);
    noecho();
    delwin(win);
}

void drawmainwin() {
    werase(MAINWIN);
    int y_size = getmaxy(MAINWIN) - 1;
    QNode* curr = CURSOR;
    for (int i = CPOS; i > 0; i--) {
        if (curr == NULL) {
            puts("cursor position too low, NULL found before y=0");
            abort();
        }
        curr = curr->prev;
    }
    for (int i = 0; i <= y_size; i++) {
        if (curr == NULL) break;

        if (CURSOR == curr) wattron(MAINWIN, A_STANDOUT);
        if (curr->n->fmode[0] == 'd'){
            wattron(MAINWIN, COLOR_PAIR(1));
            wattron(MAINWIN, A_BOLD);
        }
        if (curr->selected) {
            if (curr->n->fmode[0] == 'd')
                wattron(MAINWIN, COLOR_PAIR(2));
            else wattron(MAINWIN, COLOR_PAIR(3));
        }
        
        mvwprintw(MAINWIN, i, 0, "%s\t%s", curr->n->fmode, curr->n->fname);
        
        if (CURSOR == curr) wattroff(MAINWIN, A_STANDOUT);
        if (curr->n->fmode[0] == 'd'){
            wattroff(MAINWIN, COLOR_PAIR(1));
            wattroff(MAINWIN, A_BOLD);
        }
        if (curr->selected) {
            if (curr->n->fmode[0] == 'd')
                wattroff(MAINWIN, COLOR_PAIR(2));
            else wattroff(MAINWIN, COLOR_PAIR(3));
        }

        curr = curr->next;
    }
    
    wrefresh(MAINWIN);
}

void movecursor(int direction) {
    if (direction != 1 && direction != -1) {
        puts("wrong usage of movecursor");
        abort();
    }
    if (direction < 0) {
        if (CURSOR->prev != NULL) {
            CURSOR = CURSOR->prev;
            if (CPOS > 0) CPOS--;
        }
    }
    if (direction > 0) {
        if (CURSOR->next != NULL) {
            CURSOR = CURSOR->next;
            if (CPOS < getmaxy(MAINWIN) - 1) CPOS++;
        }
    }
}

void jumpcursor(int x) {
    while (x) {
        if (x > 0) movecursor(1);
        else movecursor(-1);
        x = (x > 0) ? x-1 : x+1;
    }
}

void handlequery() {
    qdump_unselected(&QUERYLIST);
    QUERYLIST = fzf(&DIRLIST, QUERY);
    CURSOR = QUERYLIST.start;
    CPOS = 0;
    drawmainwin();
}

void scanquery() {
    int input;
    int index = 0;
    QUERY[index] = '\0';
    mvwaddstr(BOTWIN, 0, 2, " FIND ");
    
    wmove(BOTWIN, 1, 1);
    curs_set(1);

    while (1) {
        input = wgetch(BOTWIN);
        if (input == '\n') break;
        if (input == KEY_BACKSPACE || input == 127) {
            if (index > 0) {
                wmove(BOTWIN, 1, index);
                waddch(BOTWIN, ' ');
                wmove(BOTWIN, 1, index--);
                QUERY[index] = '\0';
            }
        } else if (index < DIR_SIZE) {
            QUERY[index] = input;
            QUERY[index + 1] = '\0';
            waddch(BOTWIN, input);
            index++;
        }
        wrefresh(BOTWIN);
        handlequery();
        wmove(BOTWIN, 1, index + 1);
    }

    curs_set(0);
    werase(BOTWIN);
    box(BOTWIN, 0, 0);
    wrefresh(BOTWIN);
}

void handle_command(char* command) {
    char buffer[COMMAND_SIZE] = "\0";
    for (QNode* i = QUERYLIST.start; i != NULL; i = i->next) {
        if (i->selected != true) continue;
        parsecommand(buffer, command, i->n->fname);
        strcat(buffer, " &>/dev/null");
        system(buffer);
        buffer[0] = '\0';
    }
}

void scan_command() {
    mvwaddstr(BOTWIN, 0, 2, " ENTER COMMAND ");
    wmove(BOTWIN, 1, 1);
    curs_set(1);
    echo();

    char command[COMMAND_SIZE];
    mvwgetstr(BOTWIN, 1, 1, command);

    handle_command(command);

    noecho();
    curs_set(0);

    dump(&DIRLIST);
    qdump(&QUERYLIST);

    DIRLIST.start = DIRLIST.end = NULL;
    QUERYLIST.start = QUERYLIST.end = NULL;

    if (!listdir(&DIRLIST, 0, ".")) {
        puts("error listing directory!!!");
        abort();
    }
    QUERY[0] = '\0';
    QUERYLIST = fzf(&DIRLIST, QUERY);

    CPOS = 0;
    CURSOR = QUERYLIST.start;

    werase(MAINWIN);
    werase(BOTWIN);
    box(BOTWIN, 0, 0);
    wrefresh(BOTWIN);
    drawmainwin();
    wprintw(TOPWIN, "CWD: %s", CURDIR);
    wrefresh(TOPWIN);
    refresh();
}

void unselect_all() {
    for (QNode* i = QUERYLIST.start; i != NULL; i = i->next)
        if (i->selected == true) i->selected = false;
    drawmainwin();
}

void input_handler (int in) {
    switch (in) {
        case KEY_DOWN:
        case 'j':
            movecursor(1);
            drawmainwin();
            break;
        case KEY_UP:
        case 'k':
            movecursor(-1);
            drawmainwin();
            break;
        case 'u':
            jumpcursor(-1 * MAXLINES / 2);
            drawmainwin();
            break;
        case 'd':
            jumpcursor(MAXLINES / 2);
            drawmainwin();
            break;
        case 'f':
            scanquery();
            break;
        case ' ':
            CURSOR->selected ^= 1;
            movecursor(1);
            drawmainwin();
            break;
        case 's':
            scan_command();
            break;
        case 'c':
            unselect_all();
            break;
        case 'q':
            MAIN_LOOP = false;
            break;
    }
}