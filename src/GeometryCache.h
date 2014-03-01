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
#ifndef INFINITECACHE
    void shrinkCache();
    void refresh(long long int borderid);
    static const unsigned int CACHESIZE = 10;
#endif
    Database & db;
#ifndef INFINITECACHE
    std::list<long long int> deletequeue;
#endif
    std::unordered_map<long long int, std::vector<Way>> geometrys;
};

#endif // GEOMETRYCACHE_H
