// used for va_list in debug-print methods
#include <stdarg.h>

// file io lib
#include <stdio.h>

// getopt is used to check for the --color-flag
#include <getopt.h>
// needed for isatty (dafuq?)
#include <zlib.h>

#include "Lookup.h"
#include "stringToNumber.h"
bool usecolor = false;

void msg(const char* format, int color, va_list args) {
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
void err(const char* format, ...) {
    va_list args;
    va_start(args, format);
    msg(format, 31, args);
    va_end(args);
    exit(1);
}

// prints a formatted message to stderr, color coded to yellow
void warn(const char* format, ...) {
    va_list args;
    va_start(args, format);
    msg(format, 33, args);
    va_end(args);
}

// prints a formatted message to stderr, color coded to green
void info(const char* format, ...) {
    va_list args;
    va_start(args, format);
    msg(format, 32, args);
    va_end(args);
}

// prints a formatted message to stderr, color coded to white
void debug(const char* format, ...) {
    va_list args;
    va_start(args, format);
    msg(format, 37, args);
    va_end(args);
}

int main(int argc, char *argv[]) {
    // check if the output is a tty so we can use colors

#ifdef WIN32
    usecolor = 0;
#else
    usecolor = isatty(1);
#endif

    static struct option long_options[] = {
        {"color",                no_argument, 0, 'c'},
        {0,0,0,0}
    };

    while (1) {
        int c = getopt_long(argc, argv, "c", long_options, 0);

        if (c == -1)
            break;

        switch (c) {
            case 'c':
                usecolor = true;
                break;
            default:
                exit(1);
        }
    }

    // check for proper command line args
    if(optind != argc-2)
        err("usage: %s [--color] latitude longitude", argv[0]);

    const char* latitude = argv[optind];
    const char* longitude = argv[optind+1];

    debug("Looking up %s %s", latitude, longitude);

    Lookup l;
    std::string result = l.lookup(StringToNumber<double>(latitude), StringToNumber<double>(longitude));
    info("Match: %s", result.c_str());
}
