//
// Created by istratie-stefan on 9/2/24.
//

#ifndef CRUCEA_ROSIE_ERRORS_H
#define CRUCEA_ROSIE_ERRORS_H

char* display_error_screen(int n);
char* name_verification(char *s);
char* surname_verification(char *s);
char* age_verification_string(char *s, int n);
int age_verification_int(int age, int n);// n = 0 for adult verification, n = 1 for child verification

#endif //CRUCEA_ROSIE_ERRORS_H
