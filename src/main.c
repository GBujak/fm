#include <ncurses.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <locale.h>
#include <wchar.h>

#include "../include/const.h"
#include "../include/env.h"
#include "../include/dir.h"
#include "../include/file.h"
#include "../include/str.h"
#include "../include/ui.h"

void init() {
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    start_color();
    use_default_colors();
    curs_set(0);

    init_pair(1, COLOR_BLUE, -1);// dir
    init_pair(2, COLOR_BLUE, COLOR_YELLOW);//selected dir
    init_pair(3, COLOR_BLACK, COLOR_YELLOW);//selected

    getmaxyx(stdscr, MAXLINES, MAXCOLS);
    MAINWIN = newwin(MAXLINES - 4, MAXCOLS - 2,            1, 1);
    BOTWIN =  newwin(           3, MAXCOLS - 2, MAXLINES - 3, 1);
    TOPWIN =  newwin(           1, MAXCOLS - 2,            0, 1);

    refresh();

    keypad(stdscr, TRUE);
    keypad(BOTWIN, TRUE);

    box(BOTWIN, 0, 0);
    wrefresh(BOTWIN);

    getcwd(CURDIR, DIR_SIZE);
    wprintw(TOPWIN, "CWD: %s", CURDIR);
    wrefresh(TOPWIN);

    DIRLIST.start = NULL;
    DIRLIST.end = NULL;
    if (!listdir(&DIRLIST, 0, ".")) {
        puts("error listing directory!!!");
        abort();
    }
    QUERY[0] = '\0';
    QUERYLIST = fzf(&DIRLIST, QUERY);

    CPOS = 0;
    CURSOR = QUERYLIST.start;
    MAIN_LOOP = true;
}

int main (void) {
    int input;
    init();

    drawmainwin();

    while (MAIN_LOOP) {
        input = getch();
        input_handler(input);
    }

    endwin();
    //print_qlist(&QUERYLIST);
    dump(&DIRLIST);
    qdump(&QUERYLIST);
}