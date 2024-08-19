#include <iostream>
#include <curses.h>
#include <cstring>
#include <sqlite3.h>
#include "menu.h"
#include "search_win.h"

using namespace std;

int search_win() {
    initscr(); // Initialize the screen
    clear();
    curs_set(2);  // Set cursor visibility
    echo();
    // Display main window
    WINDOW *search_win = newwin(LINES - 13, COLS / 2, (LINES - (LINES - 13)) / 2, COLS / 4);
    refresh();
    keypad(search_win, true); // Enable keypad input
    box(search_win, 0, 0);
    mvwprintw(search_win, 0, (COLS / 2 - 13) / 2 + 1, "Search Person");
    wrefresh(search_win); // Refresh window to display title and box

    // Display buttons below the name input box
    int button_y = LINES-15; // Y-position of the buttons, adjusted to be below the name input box
    int back_button_x = ((COLS / 2) - 18) / 3 + 1;  // X-position of the Back button
    int search_button_x = ((COLS / 2) - 10) - ((COLS / 2) - 18) / 3 + 1; // X-position of the Search button

    mvwprintw(search_win, button_y, back_button_x, "[ Back ]");
    mvwprintw(search_win, button_y, search_button_x, "[ Search ]");
    wrefresh(search_win); // Refresh search_win to display buttons

    // Display surname text input box
    WINDOW* surname_input = newwin(4, (COLS / 2) - 2, (LINES - (LINES - 13)) / 2 + 1, COLS / 4 + 1);
    box(surname_input, 0, 0);
    mvwprintw(surname_input, 1, 2, "Surname: ");
    wrefresh(surname_input); // Refresh to display surname input box

    // Display name text input box
    WINDOW* name_input = newwin(4, (COLS / 2) - 2, (LINES - (LINES - 13)) / 2 + 5, COLS / 4 + 1);
    box(name_input, 0, 0);
    mvwprintw(name_input, 1, 2, "Name: ");
    wrefresh(name_input); // Refresh to display name input box


    // Init arrays for input storage
    const int MAX_LENGTH = 31;
    char name[MAX_LENGTH], surname[MAX_LENGTH];

    // Get user input
    mvwgetnstr(surname_input, 2, 2, surname, MAX_LENGTH - 1); // Get surname
    mvwgetnstr(name_input, 2, 2, name, MAX_LENGTH - 1); // Get name
    wrefresh(search_win); // Refresh search window after input
    curs_set(0);
    int ch;
    int selected = 0; // 0 -> first button, 1 -> second button
    noecho();
    while (1) {
        // Clear previous button text before redrawing them
        mvwprintw(search_win, button_y, back_button_x, "        ");
        mvwprintw(search_win, button_y, search_button_x, "          ");

        // Draw the first button at the correct position
        if (selected == 0) {
            wattron(search_win, A_REVERSE);  // Highlight first button
        }
        mvwprintw(search_win, button_y, back_button_x, "[ Back ]");
        wattroff(search_win, A_REVERSE);

        // Draw the second button at the correct position
        if (selected == 1) {
            wattron(search_win, A_REVERSE);  // Highlight second button
        }
        mvwprintw(search_win, button_y, search_button_x, "[ Search ]");
        wattroff(search_win, A_REVERSE);

        // Refresh the screen to show changes
        wrefresh(search_win);

        // Get user input
        ch = wgetch(search_win);

        // Arrow key handling to navigate between buttons
        if (ch == KEY_LEFT) {
            selected = 0;  // Select first button
        } else if (ch == KEY_RIGHT) {
            selected = 1;  // Select second button
        } else if (ch == '\n') {  // Enter key
            break;  // Exit the loop on Enter key press
        }
    }

    // Check which button was selected
    if (selected == 0)
        cout << menu(); // Back action
    else
        cout << "F-"; // Search action

    // Re-draw name input box
    //box(name_input, 0, 0);
    //wrefresh(name_input);

    // Wait for user to press 'p' key before exiting
    if (getch() == 'p')
        endwin(); // End ncurses

    // Deallocates the memory and ends ncurses
    return 0;
}
