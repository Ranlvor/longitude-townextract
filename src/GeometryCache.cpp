#include "GeometryCache.h"
#include <stdexcept>      // std::out_of_range

GeometryCache::GeometryCache(Database& d): db(d)
{
}

std::vector<Way> GeometryCache::getBorderGeometry(long long int borderid){
    std::vector<Way> geometry;
    try {
        geometry = geometrys.at(borderid);
        refresh(borderid);
    } catch (const std::out_of_range& /*oor*/) {
        GeometryCache::shrinkCache();
        geometry = db.getBorderGeometry(borderid);
        geometrys[borderid] = geometry;
        deletequeue.push_back(borderid);
    }

    return geometry;
}


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
