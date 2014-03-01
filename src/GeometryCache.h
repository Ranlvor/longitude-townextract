#ifndef GEOMETRYCACHE_H
#define GEOMETRYCACHE_H
#include "Database.h"
#include <list>
#include <unordered_map>
class GeometryCache
{
public:
    GeometryCache(Database& db);
    std::vector<Way> getBorderGeometry(long long int borderid);

private:
    void shrinkCache();
    void refresh(long long int borderid);
    static const unsigned int CACHESIZE = 10;
    Database & db;
    std::list<long long int> deletequeue;
    std::unordered_map<long long int, std::vector<Way>> geometrys;
};

#endif // GEOMETRYCACHE_H
