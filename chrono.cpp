//
// Created by istratie-stefan on 9/12/24.
//
#include <iostream>
#include <chrono>

using namespace std;

char* get_current_date() {
    // Allocate buffer for the date string (19 characters + null terminator)
    static char date_str[20]; // "YYYY-MM-DD HH:MM:SS" + '\0'

    // Get the current time point
    auto now = std::chrono::system_clock::now();

    // Convert to time_t for formatting
    time_t now_time_t = std::chrono::system_clock::to_time_t(now);

    // Convert to tm struct for local time
    tm now_tm;
#if defined(_MSC_VER) // For Windows
    localtime_s(&now_tm, &now_time_t);
#else // For Unix-based systems
    localtime_r(&now_time_t, &now_tm);
#endif

    // Format the date and time as a string
    strftime(date_str, sizeof(date_str), "%Y-%m-%d %H:%M:%S", &now_tm);

    return date_str;
}

// Get the current time
auto now = std::chrono::system_clock::now();

// Convert to time_t
time_t now_c = std::chrono::system_clock::to_time_t(now);

// Convert to tm struct
tm* localTime = std::localtime(&now_c);

int year = 1900 + localTime->tm_year;