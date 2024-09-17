#include <iostream>
#include <curses.h>
#include <cstring>
#include <sqlite3.h>
#include <sstream>
#include <iomanip>
#include "database.h"
#include "menu.h"
#include "search_win.h"
#include "errors.h"
#include "chrono.h"
#include "add_win.h"

using namespace std;

struct Input {
    int id;
    char name[31];
    char surname[31];
    int age;
    char sex[7];
    int adult_members;
    int minor_members;
    char adult_info[40];
    char minor_info[40];
    char timestamp[20];
}human;

bool two_weeks_passed(const char* timestamp) {
    // Convert the saved timestamp string into a tm structure
    tm saved_tm = {};
    istringstream ss(timestamp);
    ss >> get_time(&saved_tm, "%Y-%m-%d %H:%M:%S");

    if (ss.fail()) {
        cerr << "Failed to parse saved timestamp." << endl;
        return false;
    }

    // Convert the saved tm structure to time_t (seconds since epoch)
    time_t saved_time = mktime(&saved_tm);

    // Get the current time using your chrono function
    // Assuming you have a function that returns the current time in the same format
    string current_timestamp = get_current_date();  // Replace with actual chrono function
    tm current_tm = {};
    istringstream ss_current(current_timestamp);
    ss_current >> get_time(&current_tm, "%Y-%m-%d %H:%M:%S");

    if (ss_current.fail()) {
        cerr << "Failed to parse current timestamp." << endl;
        return false;
    }

    // Convert the current tm structure to time_t (seconds since epoch)
    time_t current_time = mktime(&current_tm);

    // Calculate the difference in seconds
    double seconds_diff = difftime(current_time, saved_time);

    // Convert 2 weeks to seconds (14 days * 24 hours * 3600 seconds)
    double two_weeks_in_seconds = 14 * 24 * 60 * 60;

    // Check if the difference is greater than or equal to 2 weeks
    return seconds_diff >= two_weeks_in_seconds;
}

int no_person_found() {
    initscr();  // Initialize the screen
    clear();
    curs_set(0);  // Hide the cursor
    noecho();     // Disable input echo

    // Create a window for the buttons
    WINDOW *button_win = newwin(5, 50, (LINES - 5) / 2 - 2, (COLS - 50) / 2);  // Position under message
    box(button_win, 0, 0);  // Add a border to the button window
    mvwprintw(button_win, 0, 45/2, "Error");
    mvwprintw(button_win, 1, 15, "No person was found.");
    keypad(button_win, true);  // Enable arrow keys for this window
    refresh();

    int selected = 0;  // 0 -> first button, 1 -> second button
    int c;

    // Initial button render
    mvwprintw(button_win, 3, 8, "[ Try again ]");
    mvwprintw(button_win, 3, 50 - 18, "[ Add Person ]");
    wrefresh(button_win);  // Refresh the button window to show the buttons

    // Event loop to handle button selection and input
    while (true) {
        // Highlight buttons based on selection
        if (selected == 0) {
            wattron(button_win, A_REVERSE);  // Highlight first button
            mvwprintw(button_win, 3, 8, "[ Try again ]");
            wattroff(button_win, A_REVERSE);
            mvwprintw(button_win, 3, 50 - 18, "[ Add Person ]");  // Unhighlight second button
        } else {
            mvwprintw(button_win, 3, 8, "[ Try again ]");  // Unhighlight first button
            wattron(button_win, A_REVERSE);  // Highlight second button
            mvwprintw(button_win, 3, 50 - 18, "[ Add Person ]");
            wattroff(button_win, A_REVERSE);
        }
        wrefresh(button_win);  // Refresh the window to show changes

        c = wgetch(button_win);  // Wait for user input
        switch (c) {
            case KEY_LEFT:
                selected = 0;  // Select the first button
                break;
            case KEY_RIGHT:
                selected = 1;  // Select the second button
                break;
            case '\n':  // Enter key
                endwin();  // End ncurses mode
                if (selected == 0) {
                    search_win();  // Call search_win for "Try again"
                    return 0;
                } else if (selected == 1) {
                    add_win();  // Call add_win for "Add Person"
                    return 1;
                }
                break;
        }
    }
}



void update_timestamp(int id, const char* new_timestamp) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int exit = sqlite3_open("database.db", &db);
    if (exit != SQLITE_OK) {
        cerr << "Error opening DB: " << sqlite3_errmsg(db) << '\n';
        return;
    }

    const char* sql = "UPDATE PERSON SET TIMESTAMP = ? WHERE ID = ?;";

    exit = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (exit != SQLITE_OK) {
        cerr << "Error preparing statement: " << sqlite3_errmsg(db) << '\n';
        sqlite3_close(db);
        return;
    }

    // Bind the new timestamp and the person ID to the SQL statement
    sqlite3_bind_text(stmt, 1, new_timestamp, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, id);

    // Execute the update query
    exit = sqlite3_step(stmt);
    if (exit != SQLITE_DONE) {
        cerr << "Error executing update: " << sqlite3_errmsg(db) << '\n';
    } else {
        cout << "Timestamp updated successfully!" << '\n';
    }

    // Finalize and close the database
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

int search(const char* name, const char* surname) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int exit = sqlite3_open("database.db", &db);
    if (exit != SQLITE_OK) {
        cerr << "Error opening DB: " << sqlite3_errmsg(db) << '\n';
        return -1;
    }

    const char* sql = "SELECT ID FROM PERSON WHERE NAME = ? AND SURNAME = ?;";
    exit = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (exit != SQLITE_OK) {
        cerr << "Error preparing statement: " << sqlite3_errmsg(db) << '\n';
        sqlite3_close(db);
        return -1;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, surname, -1, SQLITE_STATIC);

    int id = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        id = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return id;
}

void get_data(int id) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int exit = sqlite3_open("database.db", &db);
    if (exit != SQLITE_OK) {
        cerr << "Error opening DB: " << sqlite3_errmsg(db) << '\n';
        return;
    }

    const char* sql = "SELECT NAME, SURNAME, AGE, GENDER, ADULTS, CHILDREN, ADULT_INFO, CHILDREN_INFO, TIMESTAMP "
                      "FROM PERSON WHERE ID = ?;";
    exit = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (exit != SQLITE_OK) {
        cerr << "Error preparing statement: " << sqlite3_errmsg(db) << '\n';
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        strncpy(human.name, reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)), sizeof(human.name));
        strncpy(human.surname, reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)), sizeof(human.surname));
        human.age = sqlite3_column_int(stmt, 2);
        strncpy(human.sex, reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)), sizeof(human.sex));
        human.adult_members = sqlite3_column_int(stmt, 4);
        human.minor_members = sqlite3_column_int(stmt, 5);
        strncpy(human.adult_info, reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)), sizeof(human.adult_info));
        strncpy(human.minor_info, reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)), sizeof(human.minor_info));
        strncpy(human.timestamp, reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)), sizeof(human.timestamp));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}


// Search result function
int search_results(int id) {
    initscr();
    clear();
    refresh();
    // Display Result Box
    WINDOW *result_win = newwin(15, 60, (LINES-15)/2, (COLS-60)/2);
    refresh();
    box(result_win, 0, 0);
    mvwprintw(result_win, 0, 30-7, "Search Results");
    wrefresh(result_win); // Refresh window to display title and box

    // Populate Data
    get_data(id);

    // Display search results:
    if (two_weeks_passed(human.timestamp))
        mvwprintw(result_win, 1, 2, "Eligible: YES");
    else
        mvwprintw(result_win, 1, 2, "Eligible: NO");
    mvwprintw(result_win, 2, 2, "Name: %s", human.name);
    mvwprintw(result_win, 3, 2, "Surname: %s", human.surname);
    mvwprintw(result_win, 4, 2, "Age: %d", human.age);
    mvwprintw(result_win, 5, 2, "Sex: %s", human.sex);
    mvwprintw(result_win, 6, 2, "Adult Members: %d", human.adult_members);
    mvwprintw(result_win, 7, 2, "Adult Info: %s", human.adult_info);
    mvwprintw(result_win, 8, 2, "Minor Members: %d", human.minor_members);
    mvwprintw(result_win, 9, 2, "Minor Info: %s", human.minor_info);
    mvwprintw(result_win, 10, 2, "Timestamp: %s", human.timestamp);
    wrefresh(result_win);

    keypad(stdscr, TRUE);
    int c;
    int selected = 1;     // 0 -> first button, 1 -> second button

    noecho();
    curs_set(0);
    while (1) {
        mvwprintw(result_win, 13, 10, "        ");
        mvwprintw(result_win, 13, 60-18, "        ");
        // Draw the first button
        if (selected == 0) {
            wattron(result_win, A_REVERSE);  // Highlight first button
        }
        mvwprintw(result_win, 13, 10, "[ Back ]");
        wattroff(result_win, A_REVERSE);

        // Draw the second button
        if (selected == 1) {
            wattron(result_win, A_REVERSE);  // Highlight second button
        }
        mvwprintw(result_win, 13, 60-18, "[ Next ]");
        wattroff(result_win, A_REVERSE);

        // Refresh the screen to show changes
        wrefresh(result_win);
        refresh();
        // Get user input
        c = getch();
        // Arrow key handling
        if (c == KEY_LEFT) {
            selected = 0;  // Select first button
        } else if (c == KEY_RIGHT) {
            selected = 1;  // Select second button
        } else if (c == '\n') {  // Enter key
            break;
        }
        refresh();
        wrefresh(result_win);
    }

    if (selected == 0)
        menu();
    else
        update_timestamp(id, get_current_date());
        menu();
        // do the thing show the thing and menu

    endwin();

    return 0;
};

int search_win() {
    initscr(); // Initialize the screen
    clear();
    curs_set(2);  // Set cursor visibility
    echo();

    // Display main window
    WINDOW *search_win = newwin(11, COLS / 2, (LINES - (11)) / 2, COLS / 4);
    refresh();
    keypad(search_win, true); // Enable keypad input
    box(search_win, 0, 0);
    mvwprintw(search_win, 0, (COLS / 2 - 13) / 2 + 1, "Search Person");
    wrefresh(search_win); // Refresh window to display title and box

    mvwprintw(search_win, 9, 10, "[ Back ]");
    mvwprintw(search_win, 9,  COLS/2 - 18, "[ Next ]");
    wrefresh(search_win); // Refresh window to display title and box


    // Display surname text input box
    WINDOW* surname_input = newwin(4, (COLS / 2) - 2, (LINES - (11)) / 2 + 1, COLS / 4 + 1);
    box(surname_input, 0, 0);
    mvwprintw(surname_input, 1, 2, "Surname: ");
    wrefresh(surname_input); // Refresh to display surname input box

    // Display name text input box
    WINDOW* name_input = newwin(4, (COLS / 2) - 2, (LINES - (11)) / 2 + 5, COLS / 4 + 1);
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

    // Search for the person
    int id = search(name, surname);

    int button_y = 9;
    int back_button_x = 10;
    int next_button_x = COLS/2 - 18;

    int ch;
    int selected = 1; // 0 -> first button, 1 -> second button
    noecho();
    keypad(search_win, TRUE);  // Enable arrow key input for the window
    curs_set(0);  // Hide the cursor
    while (1) {
        // Clear previous button text before redrawing them
        mvwprintw(search_win, button_y, back_button_x, "        ");
        mvwprintw(search_win, button_y, next_button_x, "        ");

        // Highlight the correct button
        if (selected == 0) {
            wattron(search_win, A_REVERSE);  // Highlight first button (Back)
        }
        mvwprintw(search_win, button_y, back_button_x, "[ Back ]");
        wattroff(search_win, A_REVERSE);

        if (selected == 1) {
            wattron(search_win, A_REVERSE);  // Highlight second button (Next)
        }
        mvwprintw(search_win, button_y, next_button_x, "[ Next ]");
        wattroff(search_win, A_REVERSE);

        // Refresh the screen to show button changes
        wrefresh(search_win);


        // Get user input
        ch = wgetch(search_win);

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
        if (id != -1) {
            get_data(id); // Populate the Input struct
            search_results(id); // Display results
        } else {
            no_person_found();
        }
    }

    endwin(); // End ncurses
    return 0;
}


