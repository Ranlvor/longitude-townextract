#ifndef LOOKUP_H
#define LOOKUP_H
#include "Database.h"
#include <string>
class Lookup
{
public:
    Lookup();

    std::string lookup(double latitude, double longitude);

private:
    Database db;
};

#endif // LOOKUP_H
