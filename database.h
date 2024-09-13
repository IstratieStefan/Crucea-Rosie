//
// Created by istratie-stefan on 8/24/24.
//

#ifndef CRUCEA_ROSIE_DATABASE_H
#define CRUCEA_ROSIE_DATABASE_H
#include <sqlite3.h>

class database {

};

int register_person(sqlite3* db,
                    const char *surname,
                    const char *name,
                    int age,
                    const char *gender,
                    int adults,
                    int children,
                    int family_members,
                    const char *adult_info,
                    const char *child_info,
                    const char *timestamp);

int database_init();

extern sqlite3* db;

sqlite3* get_db();

void export_to_csv(sqlite3* db, const char* filename);

int search_person_by_name_and_surname(const char* name, const char* surname);

void display_database_info(sqlite3* db);

#endif //CRUCEA_ROSIE_DATABASE_H
