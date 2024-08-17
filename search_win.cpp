//
// Created by istratie-stefan on 8/17/24.
//
#include <iostream>
#include <curses.h>
#include <cstring>
#include "search_win.h"

using namespace std;

int search_win() {
    initscr(); // Initialises the screen
    clear();
    curs_set(2);
    //Display window
    WINDOW *search_win = newwin(LINES-14, COLS-30, 5, 15);
    refresh();
    keypad(search_win, true);
    box(search_win, 0, 0);
    wrefresh(search_win);
    mvwprintw(search_win, 0, (COLS-30-13)/2, "Search Person");
    wrefresh(search_win);

    //Display surname text input box
    WINDOW* surname_input = newwin(4, COLS - 34, 6, 17);
    box(surname_input, 0, 0);
    refresh();
    wrefresh(surname_input);
    mvwprintw(surname_input, 1, 2, "Surname: ");
    wrefresh(surname_input);

    //Display name text input box
    WINDOW* name_input = newwin(4, COLS - 34, 10, 17);
    box(name_input, 0, 0); // Draw a box around the window
    refresh();
    wrefresh(name_input);
    mvwprintw(name_input, 1, 2, "Name: ");
    wrefresh(name_input);

    //Init arrays
    const int MAX_LENGTH = 31;
    char name[MAX_LENGTH], surname[MAX_LENGTH];

    //Get user input
    mvwgetnstr(surname_input, 2, 2, surname, MAX_LENGTH - 1);
    mvwgetnstr(name_input, 2, 2, name, MAX_LENGTH - 1);

    box(name_input, 0, 0);
    wrefresh(name_input);

    // Wait for the user to press a key before exiting
    if(getch() == 'p')
        endwin();
    // Dealocates the memory and ends ncurses
    return 0;
}