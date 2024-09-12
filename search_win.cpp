#include <iostream>
#include <curses.h>
#include <cstring>
#include <sqlite3.h>
#include "database.h"
#include "menu.h"
#include "search_win.h"
#include "errors.h"

using namespace std;

struct Input {
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

    const char* sql = "SELECT NAME, SURNAME, AGE, GENDER, ADULTS, CHILDREN, ADULT_INFO, CHILDREN_INFO "
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
    WINDOW *result_win = newwin(12, 30, (LINES-12)/2, (COLS-30)/2);
    refresh();
    box(result_win, 0, 0);
    mvwprintw(result_win, 0, 7 + 1, "Search Results");
    wrefresh(result_win); // Refresh window to display title and box

    // Populate Data
    get_data(id);

    // Display search results:
    mvwprintw(result_win, 1, 2, "Eligible: ");
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
    while(1) {
        if(getch() == 'q')
            break;
    }
    menu();
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

    // Check if search was successful
    if (id != -1) {
        get_data(id); // Populate the Input struct
        search_results(id); // Display results
    } else {
        mvwprintw(search_win, 6, 2, "No results found.");
        wrefresh(search_win);
        napms(2000); // Display message for 2 seconds
    }
    endwin(); // End ncurses
    return 0;
}


