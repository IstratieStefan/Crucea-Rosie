#include <iostream>
#include <ncurses.h>
#include <cstring>
#include <sqlite3.h>
#include "add_win.h"
#include "database.h"
#include "menu.h"

using namespace std;

int database_settings() {
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

    const char* choices[] = {"Init Database", "Add Person","Delete Person", "Edit Person", "Export Database", "Option X", "Drop Database", "Back"};
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
        if (choice == 1) {

        } if (choice == 2) {
            add_win();
        } if (choice == 4) {

        } if (choice == n_choices) // Exit option selected
            menu();
    }
    // Cleanup ncurses
    delwin(menu_win); // Delete the menu window
    endwin(); // End ncurses mode

    return 0;
}