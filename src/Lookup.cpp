#include "Lookup.h"
#include "output.h"
/*
#define OSMIUM_WITH_GEOS
#include <iostream>
#include <osmium.hpp>
#include <osmium/handler/debug.hpp>
#include <osmium/osm/way.hpp>
#include <osmium/osm/relation.hpp>
#include <osmium/osm/area.hpp>
*/
Lookup::Lookup()
{
}

Lookup::~Lookup()
{
    //free(osmBorderRelation); //is a NOOP if osmBorderRelation == 0
}

std::string Lookup::lookup(double latitude, double longitude){
    debug("Lookup::lookup(%f, %f)", latitude, longitude);
    std::vector<Borderrelation> canidates = db.getPossibleBorderrelations(latitude, longitude);
    for (std::vector<Borderrelation>::iterator it = canidates.begin() ; it != canidates.end(); ++it) {
        Borderrelation & canidate = *it;
        debug("  checking border %s", canidate.name.c_str());
        loadGeometry(canidate.relationid);

        /*geometry.clear();
        Point a;
        Point b;
        Point c;
        a.id = 1;
        a.latitude = 0;
        a.longitude = 0;
        b.id = 2;
        b.latitude = 1.2;
        b.longitude = 0.9;
        c.id = 3;
        c.latitude = 1.1;
        c.longitude = -0.8;

        Way A;
        A.id = 1;
        A.points.push_back(a);
        A.points.push_back(b);
        A.points.push_back(c);
        A.points.push_back(a);

        Way B;
        B.id = 2;
        B.points.push_back(b);
        B.points.push_back(c);

        Way C;
        C.id = 3;
        C.points.push_back(c);
        C.points.push_back(a);

        geometry.push_back(A);
        //geometry.push_back(B);
        //geometry.push_back(C);*/
        if(inGeometry(latitude, longitude))
            debug("  border does match");
        else
            debug("  border does *not* match");
        return "canceled";

    }
    return canidates.begin()->name;
}


void Lookup::loadGeometry(long long int id){
    if(loadedGeometry == id)
        return;
    debug("      loading borderdata %d", id);
    geometry = db.getBorderGeometry(id);
    debug("      border has %d ways", geometry.size());

    /*free(osmBorderRelation); //is a NOOP if osmBorderRelation == 0

    osmBorderRelation = new Osmium::OSM::Relation();
    for (std::vector<Way>::iterator it = geometry.begin() ; it != geometry.end(); ++it) {
        Way & way = *it;
        if(way.type == inner)
            osmBorderRelation->add_member(NODE, way.id, "inner");
        else if (way.type == outer)
            osmBorderRelation->add_member(NODE, way.id, "outer");
    }

    free(osmBorderArea);
    osmBorderArea = new Osmium::OSM::AreaFromRelation(osmBorderRelation, true, osmBorderRelation->members().size(), 0, true);*/

    loadedGeometry = id;
}

bool Lookup::inGeometry(double latitude, double longitude) {
    int intersects = 0;
    for (std::vector<Way>::iterator it = geometry.begin() ; it != geometry.end(); ++it) {
        Way & way = *it;
        Point previous;
        previous.id = -1;
        for (std::vector<Point>::iterator it2 = way.points.begin() ; it2 != way.points.end(); ++it2) {
            Point & point = *it2;
            if(previous.id != -1) {
                debug("      checking way %d, point %d", way.id, point.id);
                if(testIntersect(latitude, longitude, previous.latitude, previous.longitude, point.latitude, point.longitude)){
                    intersects++;
                    debug("        intersects");
                }
            }
            previous = point;
        }
    }
    return (intersects % 2) == 1;
}

bool Lookup::testIntersect(double pointX, double pointY, double wayX1, double wayY1, double wayX2, double wayY2){
    debug("          testing point (%f, %f) with way (%f, %f), (%f, %f)", pointX, pointY, wayX1, wayY1, wayX2, wayY2);
    if (!(  (wayY1 <= pointY && pointY <= wayY2) //point is not in the right Y-Range, so the searchray can not intersect
         || (wayY1 >= pointY && pointY >= wayY2) )  )
        return false;

    if(wayY1 == wayY2)
        return false; //The line is horizontal, so there can be 0 or infinite matches. infinite matches should be rare and would be an intersect, 0 matches is much more common and would be no intersect. So we return no intersect and hope the infinite-matches-edgecase never happens

    double intersectXPosition = wayX1 + ((pointY-wayY1)/(wayY2-wayY1)) * (wayX2 - wayX1);
    debug("            intersectXPosition = %f", intersectXPosition);
    debug("            intersectXPosition = %f + %f * %f", wayX1, ((pointY-wayY1)/(wayY2-wayY1)),  (wayX2 - wayX1));
    if(intersectXPosition > pointX) //we only count matches right of the point, not left of the point, because our calculations made results for infinite lines but we have a searchray
        return true;
    else
        return false;
}
