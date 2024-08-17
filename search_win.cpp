//
// Created by istratie-stefan on 8/17/24.
//
#include <iostream>
#include <curses.h>

using namespace std;

int main() {
    initscr(); // Initialises the screen
    printw("adauga un om");
    int x, y;
    move(y, x);
    refresh();

    endwin(); // Dealocates the memory and ends ncurses
    return 0;
}