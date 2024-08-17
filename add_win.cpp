//
// Created by istratie-stefan on 8/17/24.
//
#include <iostream>
#include <ncurses.h>
#include "add_win.h"

using namespace std;

int birthyear(char *s) {
    int year = 1000 * s[0] + 100 * s[1] + 10 * s[2] + s[3];
    return year;
}

int add_win() {
    initscr();              // Initialize ncurses
    clear();                // Clear the screen
    cbreak();
    curs_set(2);
    //Display Left Window
    WINDOW *left_win = newwin(LINES-4, COLS/2 - 2, 2, 2);
    refresh();
    keypad(left_win, true);
    box(left_win, 0, 0);
    wrefresh(left_win);

    //Display Right Window
    WINDOW *right_win = newwin(LINES-4, COLS/2 - 2, 2, 2);
    refresh();
    keypad(right_win, true);
    box(right_win, 0, 0);
    wrefresh(right_win);

    //Display surname text input box
    WINDOW* surname_input = newwin(4, COLS/2-4, 3, 3);
    box(surname_input, 0, 0);
    refresh();
    wrefresh(surname_input);
    mvwprintw(surname_input, 1, 2, "Surname: ");
    wrefresh(surname_input);

    //Display name text input box
    WINDOW* name_input = newwin(4, COLS/2-4, 7, 3);
    box(name_input, 0, 0); // Draw a box around the window
    refresh();
    wrefresh(name_input);
    mvwprintw(name_input, 1, 2, "Name: ");
    wrefresh(name_input);

    //Display Birth Year text input box
    WINDOW* birth_input = newwin(4, COLS/2-4, 11, 3);
    box(birth_input, 0, 0); // Draw a box around the window
    refresh();
    wrefresh(birth_input);
    mvwprintw(birth_input, 1, 2, "Birth Year: ");
    wrefresh(birth_input);

    wrefresh(left_win);
    //Init arrays
    const int MAX_LENGTH = 31;
    char name[MAX_LENGTH], surname[MAX_LENGTH], birth[5];

    //Get user input
    mvwgetnstr(surname_input, 2, 2, surname, MAX_LENGTH - 1);
    mvwgetnstr(name_input, 2, 2, name, MAX_LENGTH - 1);
    mvwgetnstr(birth_input, 2, 2, birth, 4);

    int birthYear = birthyear(birth);


    box(name_input, 0, 0);
    wrefresh(name_input);
    endwin();
    return 0;
}