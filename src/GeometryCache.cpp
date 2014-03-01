#include "GeometryCache.h"
#include <stdexcept>      // std::out_of_range

GeometryCache::GeometryCache(Database& d): db(d)
{
}

std::vector<Way> GeometryCache::getBorderGeometry(long long int borderid){
    std::vector<Way> geometry;
    try {
        geometry = geometrys.at(borderid);
#ifndef INFINITECACHE
        refresh(borderid);
#endif
    } catch (const std::out_of_range& /*oor*/) {
#ifndef INFINITECACHE
        GeometryCache::shrinkCache();
#endif
        geometry = db.getBorderGeometry(borderid);
        geometrys[borderid] = geometry;
#ifndef INFINITECACHE
        deletequeue.push_back(borderid);
#endif
    }

    return geometry;
}


#ifndef INFINITECACHE
void GeometryCache::shrinkCache(){
    while (deletequeue.size() > CACHESIZE) {
        geometrys.erase(deletequeue.front());
        deletequeue.pop_front();
    }
}

void GeometryCache::refresh(long long int borderid){
    deletequeue.remove(borderid);
    deletequeue.push_back(borderid);
}
#endif
