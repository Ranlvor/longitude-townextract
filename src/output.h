#ifndef OUTPUT_H
#define OUTPUT_H
#include <stdarg.h>
void msg(const char* format, int color, va_list args);
void err(const char* format, ...);
void warn(const char* format, ...);
void info(const char* format, ...);
void debug(const char* format, ...);


#endif // OUTPUT_H
