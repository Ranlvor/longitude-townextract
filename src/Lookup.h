#ifndef LOOKUP_H
#define LOOKUP_H
#include "Database.h"
#include <string>
/*
namespace Osmium {
    namespace OSM {
        class Relation;
        class Area;
    }
}*/

class Lookup
{
public:
    Lookup();
    ~Lookup();

    std::string lookup(double latitude, double longitude);



private:
    Database db;
    long long int loadedGeometry = -1;
    std::vector<Way> geometry;
    //Osmium::OSM::Relation * osmBorderRelation = 0;
    //Osmium::OSM::Area * osmBorderArea = 0;

    void loadGeometry(long long int id);
    bool inGeometry(double latitude, double longitude);
    bool testIntersect(double pointX, double pointY, double wayX1, double wayY1, double wayX2, double wayY2);
};

#endif // LOOKUP_H
