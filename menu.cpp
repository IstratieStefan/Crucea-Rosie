#include <iostream>
#include <ncurses.h>
#include "search_win.h"
#include "add_win.h"
#include "database_win.h"
#include "statistics_menu.h"
#include "database.h"

using namespace std;

void print_menu(WINDOW* menu_win, int highlight, const char* choices[], int n_choices) {
    int x, y;
    x = 2;
    y = 2;
    box(menu_win, 0, 0);  // Draw a box around the window
    for (int i = 0; i < n_choices; ++i) {
        if (highlight == i + 1) {  // Highlight the current choice
            wattron(menu_win, A_REVERSE); // Turn on reverse coloring
            mvwprintw(menu_win, y, x, "%s", choices[i]);
            wattroff(menu_win, A_REVERSE); // Turn off reverse coloring
        } else {
            mvwprintw(menu_win, y, x, "%s", choices[i]);
        }
        ++y;
    }
    wrefresh(menu_win);  // Refresh the window to show the updates
}

int menu() {
    sqlite3* db = get_db();
    initscr();              // Initialize the ncurses mode
    clear();                // Clear the screen
    cbreak();               // Disable line buffering
    curs_set(0);            // Hide the cursor

    // Logo
    WINDOW *logo_win = newwin(6, COLS, (LINES-6)/2-8, (COLS-58)/2);
    refresh();
    mvwprintw(logo_win, 1, 0, "   ____                             ____           _      \n");
    mvwprintw(logo_win, 2, 0, "  / ___|_ __ _   _  ___ ___  __ _  |  _ \\ ___  ___(_) ___ \n");
    mvwprintw(logo_win, 3, 0, " | |   | '__| | | |/ __/ _ \\/ _` | | |_) / _ \\/ __| |/ _ \\\n");
    mvwprintw(logo_win, 4, 0, " | |___| |  | |_| | (_|  __/ (_| | |  _ < (_) \\__ \\ |  __/\n");
    mvwprintw(logo_win, 5, 0, "  \\____|_|   \\__,_|\\___\\___|\\__,_| |_| \\_\\___/|___/_|\\___|\n");
    mvwprintw(logo_win, 6, 0, "                                                          \n");
    wrefresh(logo_win);
    int startx, starty, width, height;
    height = 10;
    width = 30;
    starty = (LINES - height) / 2 + 2;  // Centering the window
    startx = (COLS - width) / 2;

    WINDOW* menu_win = newwin(height, width, starty, startx);
    keypad(menu_win, TRUE);  // Enable arrow keys in the window

    const char* choices[] = {"Search", "Add Person","Statistics", "Database Options", "Exit"};
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
            search_win();
        } if (choice == 2) {
            add_win();
        } if (choice == 3) {
            statistics_settings();
        } if (choice == 4) {
            database_settings();
        } if (choice == n_choices) // Exit option selected
            break;
    }

    clrtoeol();
    refresh();
    endwin();


    return 0;
}
