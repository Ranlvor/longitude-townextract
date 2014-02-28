
#include "Output.h"
// used for va_list in debug-print methods
#include <stdarg.h>
// file io lib
#include <stdio.h>
//contains exit()
#include <cstdlib>


bool Output::usecolor = false;

// prints a formatted message to stdout, optionally color coded
void Output::msg(const char* format, int color, va_list args) {
    if(usecolor) {
        fprintf(stdout, "\x1b[0;%dm", color);
    }
    vfprintf(stdout, format, args);
    if(usecolor) {
        fprintf(stdout, "\x1b[0m\n");
    } else {
        fprintf(stdout, "\n");
    }
}

// prints a formatted message to stderr, color coded to red
void Output::err(const char* format, ...) {
    va_list args;
    va_start(args, format);
    msg(format, 31, args);
    va_end(args);
    exit(1);
}

// prints a formatted message to stderr, color coded to yellow
void Output::warn(const char* format, ...) {
    va_list args;
    va_start(args, format);
    msg(format, 33, args);
    va_end(args);
}

// prints a formatted message to stderr, color coded to green
void Output::info(const char* format, ...) {
    va_list args;
    va_start(args, format);
    msg(format, 32, args);
    va_end(args);
}

// prints a formatted message to stderr, color coded to white
void Output::debug(const char* format, ...) {
    va_list args;
    va_start(args, format);
    msg(format, 37, args);
    va_end(args);
}