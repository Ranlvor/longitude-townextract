#ifndef OUTPUT_H
#define OUTPUT_H
#include <stdarg.h>

class Output {
public:
    static void msg(const char* format, int color, va_list args);
    static void err(const char* format, ...);
    static void warn(const char* format, ...);
    static void info(const char* format, ...);
    static void debug(const char* format, ...);

    static bool usecolor;
};

#endif // OUTPUT_H
