//
// Created by istratie-stefan on 8/18/24.
//

#ifndef CRUCEA_ROSIE_MENU_H
#define CRUCEA_ROSIE_MENU_H
#include <ncurses.h>

int menu();
void print_menu(WINDOW* menu_win, int highlight, const char* choices[], int n_choices);

#endif //CRUCEA_ROSIE_MENU_H
