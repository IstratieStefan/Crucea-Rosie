#include <iostream>
#include <ncurses.h>
#include <cstring>
#include <sqlite3.h>
#include "database.h"
#include "menu.h"
#include "statistics_win.h"

using namespace std;

sqlite3* db = get_db();

int statistics_settings() {
    // Initialize ncurses
    initscr();
    clear();
    cbreak();  // Disable line buffering
    noecho();  // Don't print user input
    keypad(stdscr, TRUE); // Enable special keys handling

    int startx, starty, width, height;
    height = 13;
    width = 30;
    starty = (LINES - height) / 2 + 2;  // Centering the window
    startx = (COLS - width) / 2;

    WINDOW* menu_win = newwin(height, width, starty, startx);
    keypad(menu_win, TRUE);  // Enable arrow keys in the window

    const char* choices[] = {"View Database", "Packages Today","Packages This Week", "Option A", "Option B", "Option C", "Option D", "Back"};
    int n_choices = sizeof(choices) / sizeof(choices[0]);
    int highlight = 1;
    int choice = 0;
    int c;

    while (true) {
        print_menu(menu_win, highlight, choices, n_choices);
        c = wgetch(menu_win);

        switch (c) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = n_choices;
                else
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight == n_choices)
                    highlight = 1;
                else
                    ++highlight;
                break;
            case 10: // Enter key
                choice = highlight;
                break;
            default:
                break;
        }
        if (choice == 0) {

        } if (choice == 1) {
             display_db_menu();
        } if (choice == 2) {

        } if (choice == 4) {

        } if (choice == n_choices) // Exit option selected
            menu();
    }
    // Cleanup ncurses
    delwin(menu_win); // Delete the menu window
    endwin(); // End ncurses mode

    return 0;
}