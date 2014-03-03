#ifndef LOOKUP_H
#define LOOKUP_H
#include "Database.h"
#include "GeometryCache.h"
#include <string>

class Lookup
{
public:
    Lookup();
    ~Lookup();

    std::string lookup(double latitude, double longitude);
    long long int getLastMatchBorderid() { return lastMatch; }


private:
    Database db;
    GeometryCache gc;
    long long int loadedGeometry = -1;
    long long int lastMatch = -1;
    std::vector<Way> geometry;

    void loadGeometry(long long int id);
    bool inGeometry(double latitude, double longitude);
    bool testIntersect(double pointX, double pointY, double wayX1, double wayY1, double wayX2, double wayY2);
};

#endif // LOOKUP_H
