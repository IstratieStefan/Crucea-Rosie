//
// Created by istratie-stefan on 9/2/24.
//

#ifndef CRUCEA_ROSIE_ERRORS_H
#define CRUCEA_ROSIE_ERRORS_H

void display_error_screen(int n);
int name_verification(char *s);
int surname_verification(char *s);
int age_verification_string(char *s);
int age_verification_int(int age, int n);// n = 0 for adult verification, n = 1 for child verification

#endif //CRUCEA_ROSIE_ERRORS_H
