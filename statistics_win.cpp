#include <ncurses.h>
#include <cstring>
#include <sqlite3.h>
#include <vector>
#include <iostream>
#include "database.h"

using namespace std;

// Callback function for formatting data as columns
int column_callback(void* data, int argc, char** argv, char** azColName) {
    vector<string>* columns = static_cast<vector<string>*>(data);

    for (int i = 0; i < argc; ++i) {
        if (i >= columns->size()) {
            columns->resize(i + 1);
        }
        columns->at(i) += (argv[i] ? argv[i] : "NULL") + string(" | ");
    }

    return 0;
}

void display_db_menu() {
    initscr();            // Initialize ncurses
    cbreak();             // Disable line buffering
    noecho();             // Do not echo user input
    keypad(stdscr, TRUE); // Enable special keys
    refresh();            // Refresh the screen

    // Prepare to get column information
    const char* sql = "PRAGMA table_info(PERSON);";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        endwin();
        cerr << "Failed to query table info: " << sqlite3_errmsg(db) << endl;
        return;
    }

    vector<int> column_widths;
    vector<string> column_headers;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        column_widths.push_back(0); // Initialize column width for each column
        column_headers.push_back((const char*)sqlite3_column_text(stmt, 1));
    }
    sqlite3_finalize(stmt);

    // Query data
    sql = "SELECT * FROM PERSON;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        endwin();
        cerr << "Failed to query table data: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Determine the maximum width for each column
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        for (int i = 0; i < column_widths.size(); ++i) {
            int length = strlen((const char*)sqlite3_column_text(stmt, i));
            if (length > column_widths[i]) {
                column_widths[i] = length;
            }
        }
    }
    sqlite3_finalize(stmt);

    // Create a window for displaying data
    WINDOW* win = newwin(LINES, COLS, 0, 0);
    box(win, 0, 0); // Draw a border around the window

    // Display column headers
    int x = 1;
    for (size_t i = 0; i < column_headers.size(); ++i) {
        mvwprintw(win, 0, x, "%-*s", column_widths[i], column_headers[i].c_str());
        x += column_widths[i] + 2; // Add space for the vertical line
    }

    // Draw vertical lines between columns
    x = column_widths[0] + 1; // Initial position for the first line
    for (size_t i = 1; i < column_headers.size(); ++i) {
        for (int y = 1; y < LINES - 2; ++y) {
            mvwaddch(win, y, x, ACS_VLINE); // Use ACS_VLINE for vertical line
        }
        x += column_widths[i] + 1; // Move to the next line position
    }

    // Display data
    int y = 1; // Start from the second line
    sql = "SELECT * FROM PERSON;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int x = 1;
            for (int i = 0; i < column_widths.size(); ++i) {
                const char* text = (const char*)sqlite3_column_text(stmt, i);
                mvwprintw(win, y, x, "%-*s", column_widths[i], text ? text : "NULL");
                x += column_widths[i] + 1; // Move to the next column with space for the vertical line
            }
            y++;
            if (y >= LINES - 1) {
                break; // Stop if there's no more space
            }
        }
    }
    sqlite3_finalize(stmt);

    mvwprintw(win, LINES - 1, 1, "Press any key to exit...");
    wrefresh(win);
    getch();
    delwin(win);

    endwin(); // Cleanup ncurses
}
