#include <ncurses.h>
#include <cstring>
#include <sqlite3.h>
#include <iostream>
#include "database.h"
#include "statistics_menu.h"

using namespace std;

const int NUM_COLUMNS = 11; // Number of columns

void display_db_menu() {
    initscr();            // Initialize ncurses
    cbreak();             // Disable line buffering
    noecho();             // Do not echo user input
    keypad(stdscr, TRUE); // Enable special keys

    // Create a window for displaying data
    WINDOW* win = newwin(LINES, COLS, 0, 0);
    box(win, 0, 0); // Draw a border around the window
    scrollok(win, TRUE); // Enable scrolling within the window
    idlok(win, TRUE);    // Enable software line-drawing for the window

    // Column headers and widths
    const char* headers[NUM_COLUMNS] = {
            "ID", "Surname", "Name", "Age", "Gender", "A", "C", "F", "Adults", "Children", "Timestamp"
    };
    int column_widths[NUM_COLUMNS];
    memset(column_widths, 0, sizeof(column_widths)); // Initialize widths to 0

    // Query to get column widths
    const char* sql = "SELECT * FROM PERSON;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        endwin();
        cerr << "Failed to query table data: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Determine the maximum width for each column
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        for (int i = 0; i < NUM_COLUMNS; ++i) {
            int length = strlen((const char*)sqlite3_column_text(stmt, i));
            if (length > column_widths[i]) {
                column_widths[i] = length;
            }
        }
    }
    sqlite3_finalize(stmt);

    // Display column headers
    int x = 1;
    for (int i = 0; i < NUM_COLUMNS; ++i) {
        mvwprintw(win, 0, x, "%-*s", column_widths[i], headers[i]);
        x += column_widths[i] + 1; // Add space between columns
    }

    // Draw vertical lines between columns
    x = column_widths[0] + 1; // Initial position for the first line
    for (int i = 1; i < NUM_COLUMNS; ++i) {
        for (int y = 1; y < LINES - 2; ++y) {
            mvwaddch(win, y, x, ACS_VLINE); // Use ACS_VLINE for vertical line
        }
        x += column_widths[i] + 1; // Move to the next line position
    }

    // Display data
    int y = 1; // Start from the second line
    int scroll_position = 0; // Initial scroll position
    int data_count = 0; // Total number of rows in the database

    // Count the total number of rows
    sql = "SELECT COUNT(*) FROM PERSON;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        data_count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    bool running = true;
    while (running) {
        // Clear previous data
        werase(win);

        // Display column headers and vertical lines
        box(win, 0, 0); // Draw border around window
        x = 1;
        for (int i = 0; i < NUM_COLUMNS; ++i) {
            mvwprintw(win, 0, x, "%-*s", column_widths[i], headers[i]);
            x += column_widths[i] + 1;
        }
        x = column_widths[0] + 1;
        for (int i = 1; i < NUM_COLUMNS; ++i) {
            for (int y = 1; y < LINES - 2; ++y) {
                mvwaddch(win, y, x, ACS_VLINE);
            }
            x += column_widths[i] + 1;
        }

        // Display data based on the current scroll position
        y = 1; // Start from the second line
        sql = "SELECT * FROM PERSON LIMIT ? OFFSET ?;";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, LINES - 3); // Number of rows to display
            sqlite3_bind_int(stmt, 2, scroll_position); // Offset
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                x = 1;
                for (int i = 0; i < NUM_COLUMNS; ++i) {
                    const char* text = (const char*)sqlite3_column_text(stmt, i);
                    mvwprintw(win, y, x, "%-*s", column_widths[i], text ? text : "NULL");
                    x += column_widths[i] + 1;
                }
                y++;
            }
        }
        sqlite3_finalize(stmt);

        mvwprintw(win, LINES - 1, 1, "Press 'q' to exit, 'UP' to scroll up, 'DOWN' to scroll down...");
        wrefresh(win);

        int ch = getch();
        switch (ch) {
            case 'q':
                running = false;
                break;
            case KEY_DOWN: // Scroll down
                if (scroll_position + (LINES - 3) < data_count) {
                    scroll_position += (LINES - 3);
                }
                break;
            case KEY_UP: // Scroll up
                if (scroll_position - (LINES - 3) >= 0) {
                    scroll_position -= (LINES - 3);
                }
                break;
        }
    }

    delwin(win);
    endwin(); // Cleanup ncurses
}
