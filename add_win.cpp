//
// Created by istratie-stefan on 8/17/24.
//
#include <iostream>
#include <ncurses.h>
#include <cstdio>
#include <chrono>
#include <cstring>
#include <sqlite3.h>
#include "add_win.h"
#include "menu.h"
#include "database.h"
#include "errors.h"
#include "chrono.h"

using namespace std;

int ageToBirthYear(char *s) {
    int age = atoi(s);
    return year - age;
}

int age(char *s) {
    int x;
    if (strlen(s) <= 3) {
        x = ageToBirthYear(s);
    }
    else {
        x = atoi(s);
    }
    return x;
}

int add_win() {
    initscr();              // Initialize ncurses
    clear();                // Clear the screen
    cbreak();
    curs_set(2);
    echo();

    //Display Window Fame
    WINDOW *window_frame = newwin(24, COLS/2, (LINES-22)/2, (COLS/4) );
    refresh();
    keypad(window_frame, true);
    box(window_frame, 0, 0);
    mvwprintw(window_frame, 0 ,(COLS/2-10)/2, "Add Person");
    wrefresh(window_frame);

    //Display surname text input box
    WINDOW* surname_input = newwin(4, COLS/2-4, (LINES-22)/2+1, (COLS/4)+2);
    box(surname_input, 0, 0);
    refresh();
    wrefresh(surname_input);
    mvwprintw(surname_input, 1, 2, "Surname: ");
    wrefresh(surname_input);

    //Display name text input box
    WINDOW* name_input = newwin(4, COLS/2-4, (LINES-22)/2+5, (COLS/4)+2);
    box(name_input, 0, 0); // Draw a box around the window
    refresh();
    wrefresh(name_input);
    mvwprintw(name_input, 1, 2, "Name: ");
    wrefresh(name_input);

    //Display Birth Year text input box
    WINDOW* birth_input = newwin(4, COLS/2-4, (LINES-22)/2+9, (COLS/4)+2);
    box(birth_input, 0, 0); // Draw a box around the window
    refresh();
    wrefresh(birth_input);
    mvwprintw(birth_input, 1, 2, "Birth Year / Age: ");
    wrefresh(birth_input);

    //Display Adult Family members text input box
    WINDOW* adults_input = newwin(4, COLS/2-4, (LINES-22)/2+13, (COLS/4)+2);
    box(adults_input, 0, 0); // Draw a box around the window
    refresh();
    wrefresh(adults_input);
    mvwprintw(adults_input, 1, 2, "Adult Family members:  ");
    wrefresh(adults_input);

    //Display Minor Family members text input box
    WINDOW* minor_input = newwin(4, COLS/2-4, (LINES-22)/2+17, (COLS/4)+2);
    box(minor_input, 0, 0); // Draw a box around the window
    refresh();
    wrefresh(minor_input);
    mvwprintw(minor_input, 1, 2, "Minor Family members:  ");
    wrefresh(minor_input);

    //Displays buttons
    mvwprintw(window_frame, 22, ((COLS /2) - 18)/3+1, "[ Male ]");//8
    wrefresh(window_frame);
    mvwprintw(window_frame, 22, ((COLS/2)-10)-((COLS /2) - 18)/3+1, "[ Female ]");//10
    wrefresh(window_frame);
    //Init arrays
    const int MAX_LENGTH = 31;
    char name[MAX_LENGTH], surname[MAX_LENGTH], birth[5], adults[2], kids[2], adultData[65] = {0}, kidData[65] = {0}, gender[7];

    //Get user input
    mvwgetnstr(surname_input, 2, 2, surname, MAX_LENGTH - 1);
    mvwgetnstr(name_input, 2, 2, name, MAX_LENGTH - 1);
    mvwgetnstr(birth_input, 2, 2, birth, 4);
    mvwgetnstr(adults_input, 2, 2, adults, 1);
    mvwgetnstr(minor_input, 2, 2, kids, 1);

    int c;
    int selected = 0;     // 0 -> first button, 1 -> second button

    noecho();
    curs_set(0);
    while (1) {
        mvwprintw(window_frame, 22, ((COLS /2) - 18)/3+1, "        ");
        mvwprintw(window_frame, 22, ((COLS/2)-10)-((COLS /2) - 18)/3+1, "          ");
        // Draw the first button
        if (selected == 0) {
            wattron(window_frame, A_REVERSE);  // Highlight first button
        }
        mvwprintw(window_frame, 22, ((COLS /2) - 18)/3+1, "[ Male ]");
        wattroff(window_frame, A_REVERSE);

        // Draw the second button
        if (selected == 1) {
            wattron(window_frame, A_REVERSE);  // Highlight second button
        }
        mvwprintw(window_frame, 22, ((COLS/2)-10)-((COLS /2) - 18)/3+1, "[ Female ]");
        wattroff(window_frame, A_REVERSE);

        // Refresh the screen to show changes
        wrefresh(window_frame);

        // Get user input
        c = wgetch(window_frame);
        // Arrow key handling
        if (c == KEY_LEFT) {
            selected = 0;  // Select first button
        } else if (c == KEY_RIGHT) {
            selected = 1;  // Select second button
        } else if (c == '\n') {  // Enter key
            break;  // Next Adult starts
        }
    }

    if (selected == 0)
        strcpy(gender, "Male");
    else
        strcpy(gender, "Female");



    // Input verification
    strcpy(surname, surname_verification(surname));
    strcpy(name, name_verification(name));

    // Birth calculation
    int birthYear = 0;
    birthYear = age(birth);

    //age_verification_int(year - birthYear, 0);

    // Counts
    int adultsCount = atoi(adults);
    int kidsCount = atoi(kids);
    int familyCount = adultsCount + kidsCount;

    if (strcmp(gender, "Male") == 0)
        strcat(adultData, "M");
    else
        strcat(adultData, "F");
    char age_year[5];
    sprintf(age_year, "%d", birthYear);
    strcat(adultData, age_year);
    if (adultsCount > 1)
        strcat(adultData, ",");

    // Adult input
    for (int i = 1; i < adultsCount; i++) {
        clear();
        curs_set(1);
        echo();
        char input[5];
        WINDOW * adult_window = newwin(7, COLS/2, (LINES-8)/2, (COLS/4));
        refresh();
        keypad(adult_window, true);
        box(adult_window, 0, 0);
        mvwprintw(adult_window, 0 ,(COLS/2-10)/2, "Adult %d", i+1);
        wrefresh(adult_window);

        //Displays text box
        WINDOW* age_input = newwin(4, COLS/2-4, (LINES-8)/2+1, (COLS/4)+2);
        box(age_input, 0, 0);
        refresh();
        wrefresh(age_input);
        mvwprintw(age_input, 1, 2, "Birth Year / Age: ");
        wrefresh(age_input);

        //Displays buttons
        mvwprintw(adult_window, 5, ((COLS /2) - 18)/3+1, "[ Male ]");//8
        wrefresh(adult_window);
        mvwprintw(adult_window, 5, ((COLS/2)-10)-((COLS /2) - 18)/3+1, "[ Female ]");//10
        wrefresh(adult_window);

        mvwgetnstr(age_input, 2, 2, input, MAX_LENGTH - 1);
        wrefresh(adult_window);
        curs_set(0);
        int ch;
        int selected = 0;     // 0 -> first button, 1 -> second button

        noecho();
        while (1) {
            mvwprintw(adult_window, 5, ((COLS /2) - 18)/3+1, "        ");
            mvwprintw(adult_window, 5, ((COLS/2)-10)-((COLS /2) - 18)/3+1, "          ");
            // Draw the first button
            if (selected == 0) {
                wattron(adult_window, A_REVERSE);  // Highlight first button
            }
            mvwprintw(adult_window, 5, ((COLS /2) - 18)/3+1, "[ Male ]");
            wattroff(adult_window, A_REVERSE);

            // Draw the second button
            if (selected == 1) {
                wattron(adult_window, A_REVERSE);  // Highlight second button
            }
            mvwprintw(adult_window, 5, ((COLS/2)-10)-((COLS /2) - 18)/3+1, "[ Female ]");
            wattroff(adult_window, A_REVERSE);

            // Refresh the screen to show changes
            wrefresh(adult_window);

            // Get user input
            ch = wgetch(adult_window);
            // Arrow key handling
            if (ch == KEY_LEFT) {
                selected = 0;  // Select first button
            } else if (ch == KEY_RIGHT) {
                selected = 1;  // Select second button
            } else if (ch == '\n') {  // Enter key
               break;  // Next Adult starts
            }
        }
        if (selected == 0)
            strcat(adultData, "M");
        else
            strcat(adultData, "F");
        int birthYearAdult = age(input);
        char x[5];
        sprintf(x, "%d", birthYearAdult);
        strcat(adultData,x);
        if (i != adultsCount-1)
            strcat(adultData,",");
    }

    //Kids input

    for (int i = 0; i < kidsCount; i++) {
        clear();
        curs_set(1);
        echo();
        char input[5];
        WINDOW * adult_window = newwin(7, COLS/2, (LINES-8)/2, (COLS/4));
        refresh();
        keypad(adult_window, true);
        box(adult_window, 0, 0);
        mvwprintw(adult_window, 0 ,(COLS/2-10)/2, "Child %d", i+1);
        wrefresh(adult_window);

        //Displays text box
        WINDOW* age_input = newwin(4, COLS/2-4, (LINES-8)/2+1, (COLS/4)+2);
        box(age_input, 0, 0);
        refresh();
        wrefresh(age_input);
        mvwprintw(age_input, 1, 2, "Birth Year / Age: ");
        wrefresh(age_input);

        //Displays buttons
        mvwprintw(adult_window, 5, ((COLS /2) - 18)/3+1, "[ Male ]");//8
        wrefresh(adult_window);
        mvwprintw(adult_window, 5, ((COLS/2)-10)-((COLS /2) - 18)/3+1, "[ Female ]");//10
        wrefresh(adult_window);

        mvwgetnstr(age_input, 2, 2, input, MAX_LENGTH - 1);
        wrefresh(adult_window);
        curs_set(0);
        int ch;
        int selected = 0;     // 0 -> first button, 1 -> second button

        noecho();
        while (1) {
            mvwprintw(adult_window, 5, ((COLS /2) - 18)/3+1, "        ");
            mvwprintw(adult_window, 5, ((COLS/2)-10)-((COLS /2) - 18)/3+1, "          ");
            // Draw the first button
            if (selected == 0) {
                wattron(adult_window, A_REVERSE);  // Highlight first button
            }
            mvwprintw(adult_window, 5, ((COLS /2) - 18)/3+1, "[ Male ]");
            wattroff(adult_window, A_REVERSE);

            // Draw the second button
            if (selected == 1) {
                wattron(adult_window, A_REVERSE);  // Highlight second button
            }
            mvwprintw(adult_window, 5, ((COLS/2)-10)-((COLS /2) - 18)/3+1, "[ Female ]");
            wattroff(adult_window, A_REVERSE);

            // Refresh the screen to show changes
            wrefresh(adult_window);

            // Get user input
            ch = wgetch(adult_window);
            // Arrow key handling
            if (ch == KEY_LEFT) {
                selected = 0;  // Select first button
            } else if (ch == KEY_RIGHT) {
                selected = 1;  // Select second button
            } else if (ch == '\n') {  // Enter key
                break;  // Next Adult starts
            }
        }
        if (selected == 0)
            strcat(kidData, "M");
        else
            strcat(kidData, "F");
        int birthYearAdult = age(input);
        char x[5];
        sprintf(x, "%d", birthYearAdult);
        strcat(kidData,x);
        if (i != kidsCount-1)
            strcat(kidData,",");
    }

    register_person(db, surname, name, birthYear, gender, adultsCount, kidsCount, familyCount, adultData, kidData, get_current_date());
    //display_database_info(sqlite3* db);
    endwin();
    menu();
    return 0;
}