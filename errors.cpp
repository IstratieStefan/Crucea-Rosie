#include <iostream>
#include <ncurses.h>
#include <cstring>
#include <chrono>
#include "menu.h"

using namespace std;
// code 1 - name
// code 2 - surname
// code 3 - birthyear/age
// code 4 - birthyear/age too small or too big

char* name_verification(char *s);
char* surname_verification(char *s);
char* age_verification_string(char *s, int n);
int age_verification_int(int age, int n);


char* display_error_screen(int n) {
    initscr();              // Initialize ncurses
    clear();                // Clear the screen
    cbreak();
    curs_set(2);
    echo();

    char* s = new char[31];

    // Error_window size
    int x_size = 60;
    int y_size = 10;

    // Display buttons below the name input box
    int button_y = 8; // Y-position of the buttons, adjusted to be below the name input box
    int back_button_x = ((COLS - x_size)/2) + 1;  // X-position of the Back button
    int next_button_x = ((COLS - x_size)/2) + 8; // X-position of the Search button

    //Error Window
    WINDOW* error_win = newwin(y_size, x_size , (LINES -y_size)/2, (COLS - x_size)/2);
    box(error_win, 0, 0);
    refresh();
    wrefresh(error_win);
    switch(n) {
        case 1:
            mvwprintw(error_win, 1, 2, "Invalid name. Name can't contain \n  numbers or special characters.");
            break;
        case 2:
            mvwprintw(error_win, 1, 2, "Invalid surname. Surname can't contain \n  numbers or special characters.");
            break;
        case 3:
            mvwprintw(error_win, 1, 2, "Invalid Age. Age can't contain \n  numbers or special characters.");
            break;
        case 4:
            mvwprintw(error_win, 1, 2, "Invalid Age. Please enter a valid age.");
            break;
        default:
            mvwprintw(error_win, 1, 2, "An error occurred.");
    }
    box(error_win, 0, 0);
    wrefresh(error_win);

    mvwprintw(error_win, button_y, back_button_x, "[ Back ]");
    mvwprintw(error_win, button_y, next_button_x, "[ Next ]");
    wrefresh(error_win); // Refresh search_win to display buttons
    refresh(); // Refresh search_win to display updated buttons

    // New input
    WINDOW* new_input = newwin(4, x_size-2, (LINES - y_size)/2+4, (COLS - x_size)/2+1);
    box(new_input, 0, 0);
    refresh();
    wrefresh(new_input);
    switch(n) {
        case 1:
            mvwprintw(new_input, 1, 2, "Corrected name:");
            break;
        case 2:
            mvwprintw(new_input, 1, 2, "Corrected surname");
            break;
        case 3:
            mvwprintw(new_input, 1, 2, "Corrected age");
            break;
        case 4:
            mvwprintw(new_input, 1, 2, "Corrected age");
            break;
        default:
            mvwprintw(new_input, 1, 2, "An error occurred.");
    }
    wrefresh(new_input);

    mvwgetnstr(new_input, 2, 2, s, 30);
    int ch;
    int selected = 0; // 0 -> first button, 1 -> second button
    noecho();
    keypad(error_win, TRUE);  // Enable arrow key input for the window
    while (1) {
        // Clear previous button text before redrawing them
        mvwprintw(error_win, button_y, back_button_x, "        ");
        mvwprintw(error_win, button_y, next_button_x, "        ");

        // Highlight the correct button
        if (selected == 0) {
            wattron(error_win, A_REVERSE);  // Highlight first button (Back)
        }
        mvwprintw(error_win, button_y, back_button_x, "[ Back ]");
        wattroff(error_win, A_REVERSE);

        if (selected == 1) {
            wattron(error_win, A_REVERSE);  // Highlight second button (Next)
        }
        mvwprintw(error_win, button_y, next_button_x, "[ Next ]");
        wattroff(error_win, A_REVERSE);

        // Refresh the screen to show button changes
        wrefresh(error_win);


        // Get user input
        ch = wgetch(error_win);

        // Arrow key handling to navigate between buttons
        if (ch == KEY_LEFT || ch == KEY_RIGHT) {
            selected = 1 - selected;  // Toggle between 0 (Back) and 1 (Next)
        } else if (ch == '\n') {  // Enter key
            break;  // Exit the loop on Enter key press
        }
    }

    // Handle selected button action
    if (selected == 0) {
        menu();  // Call menu function for Back action
    } else {
        if (n == 1) {
            name_verification(s);
        } else if (n == 2) {
            surname_verification(s);
        } else if (n == 3) {
            age_verification_string(s, n);
        }
        return s;
    }
    //endwin();
}

char* name_verification(char *s) {
    char* a = new char[31];
    bool error_found = false;

    // Check if there are invalid characters in the name
    for (int i = 0; s[i] != '\0'; i++) {
        if (strchr("0123456789 .,?/';{}[]<>!@#$%^&*", s[i])) {
            strcpy(a, display_error_screen(1));  // Ask user to correct the name
            error_found = true;
            break;  // Exit the loop after finding an error
        }
    }

    if (!error_found) {
        // No error found, copy the original name
        strcpy(a, s);
    }

    return a;  // Return the corrected or original name
}


char* surname_verification(char *s) {
    char* a = new char[31];
    bool error_found = false;

    // Check if there are invalid characters in the surname
    for (int i = 0 ; s[i] != '\0'; i++) {
        if (strchr("0123456789 .,?/';{}[]<>!@#$%^&*", s[i])) {
            strcpy(a, display_error_screen(2));  // Ask user to correct the surname
            error_found = true;
            break;  // Exit the loop after finding an error
        }
    }

    if (!error_found) {
        // No error found, copy the original surname
        strcpy(a, s);
    }

    return a;  // Return the corrected or original surname
}

char* age_verification_string(char *s, int n) {
    char* a = new char[31];
    bool error_found = false;

    // Check if there are invalid characters in the age (string format)
    for (int i = 0 ; s[i] != '\0'; i++) {
        if (strchr(" .,?/';{}[]<>!@#$%^&*", s[i])) {
            strcpy(a, display_error_screen(3));  // Ask user to correct the age
            error_found = true;
            break;  // Exit the loop after finding an error
        }
    }

    if (!error_found) {
        // No error found, copy the original age string
        strcpy(a, s);

        // Convert the string to an integer and validate the age range
        int age = atoi(a);  // Convert the string to an integer
        if (age == 0 && strcmp(a, "0") != 0) {
            strcpy(a, display_error_screen(3));  // Invalid input, ask for correction again
        } else {
            age_verification_int(age, n);  // Verify the integer age, assuming '0' means checking for minor (adjust n as needed)
        }
    }

    return a;  // Return the corrected or original age string
}



int age_verification_int(int age, int n) {
    if (n == 0) {
        if (age < 18)
            display_error_screen(4);
    }
    else {
        if (age >= 18)
            display_error_screen(4);
    }
    return 0;
}



