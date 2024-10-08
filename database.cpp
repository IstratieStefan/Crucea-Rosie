#include <iostream>
#include <sqlite3.h>
#include <sstream>
#include <fstream>

using namespace std;

void drop_all_tables(sqlite3* db) {
    const char* drop_table_sql = "DROP TABLE IF EXISTS PERSON;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, drop_table_sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare drop table statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cerr << "Failed to execute drop table statement: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    cout << "All tables have been dropped." << endl;
}

// Callback function for displaying data
int callback(void* data, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) {
        cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << endl;
    }
    cout << endl;
    return 0;
}

// Returns the sqlite3 pointer
sqlite3* get_db() {
    sqlite3* db;
    int exit = sqlite3_open("database.db", &db);
    if (exit != SQLITE_OK) {
        cerr << "Error opening DB: " << sqlite3_errmsg(db) << endl;
        return nullptr;  // Return null if there was an error
    }
    return db;  // Return the valid sqlite3* database pointer
}

// Function to register a person in the database
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
                    const char *timestamp) {

    stringstream ss;
    ss << "INSERT INTO PERSON (SURNAME, NAME, AGE, GENDER, ADULTS, CHILDREN, FAMILY_MEMBERS, ADULT_INFO, CHILDREN_INFO, TIMESTAMP) "
          "VALUES('" << surname << "', '" << name << "', " << age << ", '" << gender << "', "
       << adults << ", " << children << ", " << family_members << ", '" << adult_info << "', '" << child_info
       << "', '" << timestamp << "');";
    string sql = ss.str();

    char* messageError = nullptr;
    int exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        cerr << "Error Insert: " << messageError << '\n';
        sqlite3_free(messageError);
        return exit; // Return the error code
    } else {
        cout << "Record created successfully!" << '\n';
    }

    return 0; // Return 0 on success
}

// Function to export database to a CSV file
void export_to_csv(sqlite3* db, const char* filename) {
    ofstream csv_file(filename);
    if (!csv_file.is_open()) {
        cerr << "Could not open file: " << filename << '\n';
        return;
    }

    const char* sql = "SELECT * FROM PERSON;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        int col_count = sqlite3_column_count(stmt);

        // Write the column names to the CSV file
        for (int i = 0; i < col_count; i++) {
            csv_file << sqlite3_column_name(stmt, i);
            if (i < col_count - 1) csv_file << ",";
        }
        csv_file << "\n";

        // Write the data rows to the CSV file
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            for (int i = 0; i < col_count; i++) {
                const char* col_text = (const char*)sqlite3_column_text(stmt, i);
                csv_file << (col_text ? col_text : "NULL");
                if (i < col_count - 1) csv_file << ",";
            }
            csv_file << "\n";
        }
    } else {
        cerr << "Failed to execute query to export data" << '\n';
    }

    sqlite3_finalize(stmt);
    csv_file.close();
    cout << "Data exported to " << filename << " successfully!" << '\n';
}

// Function to initialize the database
int database_init() {
    sqlite3* db;  // Declare the db pointer
    int exit = sqlite3_open("database.db", &db);  // Now, the db is assigned properly

    if (exit != SQLITE_OK) {
        cerr << "Error opening DB: " << sqlite3_errmsg(db) << '\n';
        return -1;
    } else {
        cout << "Opened Database Successfully!" << '\n';
    }

    string sql = "CREATE TABLE IF NOT EXISTS PERSON("
                 "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "SURNAME TEXT NOT NULL, "
                 "NAME TEXT NOT NULL, "
                 "AGE INT NOT NULL, "
                 "GENDER TEXT NOT NULL, "
                 "ADULTS INT NOT NULL, "
                 "CHILDREN INT NOT NULL, "
                 "FAMILY_MEMBERS INT NOT NULL, "
                 "ADULT_INFO TEXT NOT NULL, "
                 "CHILDREN_INFO TEXT NOT NULL, "
                 "TIMESTAMP TEXT NOT NULL);";

    char* messageError = nullptr;
    exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        cerr << "Error Creating Table: " << messageError << '\n';
        sqlite3_free(messageError);
    } else {
        cout << "Table created Successfully" << '\n';
    }

    sqlite3_close(db);  // Close the database connection
    return 0;
}

int database_main() {
    sqlite3* db;
    int exit = sqlite3_open("database.db", &db);

    if (exit == SQLITE_OK) {
        // Initialize the database (create table, insert data, export to CSV)
        database_init();

        // Display database information
        //display_database_info(db);

        sqlite3_close(db);
    } else {
        cerr << "Error opening DB: " << sqlite3_errmsg(db) << '\n';
    }

    return 0;
}
