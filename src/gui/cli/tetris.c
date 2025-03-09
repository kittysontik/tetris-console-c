#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main()
{
    // int argc, char**argv

    initscr();
    printw("Hello");
    refresh();
    getch();
    endwin();
    return 0;
}