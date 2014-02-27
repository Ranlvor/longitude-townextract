#include "Lookup.h"
#include "output.h"

Lookup::Lookup()
{
}

Lookup::~Lookup()
{
}

std::string Lookup::lookup(double latitude, double longitude){
    debug("Lookup::lookup(%f, %f)", latitude, longitude);
    std::vector<Borderrelation> canidates = db.getPossibleBorderrelations(latitude, longitude);
    for (std::vector<Borderrelation>::iterator it = canidates.begin() ; it != canidates.end(); ++it) {
        Borderrelation & canidate = *it;
        debug("  checking border %s", canidate.name.c_str());
        loadGeometry(canidate.relationid);

        if(inGeometry(latitude, longitude))
            debug("  border does match");
        else
            debug("  border does *not* match");
    }
    return canidates.begin()->name;
}


void Lookup::loadGeometry(long long int id){
    if(loadedGeometry == id)
        return;
    debug("      loading borderdata %d", id);
    geometry = db.getBorderGeometry(id);
    debug("      border has %d ways", geometry.size());

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
                debug("      checking way %d, point %d,  data: point(%f, %f), way1 (%f, %f), way 2 (%f, %f)", way.id, point.id, latitude, longitude, previous.latitude, previous.longitude, point.latitude, point.longitude);
                if(testIntersect(latitude, longitude, previous.latitude, previous.longitude, point.latitude, point.longitude)){
                    intersects++;
                    debug("      intersection: way %d, point %d", way.id, point.id);
                    debug("      arguments where point(%f, %f), way1 (%f, %f), way 2 (%f, %f)", latitude, longitude, previous.latitude, previous.longitude, point.latitude, point.longitude);
                }
            }
            previous = point;
        }
    }
    debug("%d intersects", intersects);
    return (intersects % 2) == 1;
}

bool Lookup::testIntersect(double pointX, double pointY, double wayX1, double wayY1, double wayX2, double wayY2){
    //debug("          testing point (%f, %f) with way (%f, %f), (%f, %f)", pointX, pointY, wayX1, wayY1, wayX2, wayY2);
    if (!(  (wayY1 <= pointY && pointY <= wayY2) //point is not in the right Y-Range, so the searchray can not intersect
         || (wayY1 >= pointY && pointY >= wayY2) )  )
        return false;

    if(wayY1 == wayY2 || wayX1 == wayX2)
        return false; //The line is horizontal or vertical, so there can be 0 or infinite matches. infinite matches should be rare and would be an intersect, 0 matches is much more common and would be no intersect. So we return no intersect and hope the infinite-matches-edgecase never happens

    double yOffset = ((pointY-wayY1)/(wayY2-wayY1));
    //test deactivead because this is already tested with the first test: the Y range test
    //if(yOffset < 0 || yOffset > 1)
    //    return false; //intersection is outside of the tested line

    double intersectXPosition = wayX1 + yOffset * (wayX2 - wayX1);
    //debug("            intersectXPosition = %f", intersectXPosition);
    //debug("            intersectXPosition = %f + %f * %f", wayX1, ((pointY-wayY1)/(wayY2-wayY1)),  (wayX2 - wayX1));
    if(intersectXPosition < pointX) { //we only count matches right of the point, not left of the point, because our calculations made results for infinite lines but we have a searchray
        debug("intersection data: %f = %f + %f * %f", intersectXPosition, wayX1, ((pointY-wayY1)/(wayY2-wayY1)), (wayX2 - wayX1));
        debug("intersection data: %f / %f", (pointY-wayY1),(wayY2-wayY1));
        return true;
    } else
        return false;
}


//Code from http://www.spieleprogrammierer.de/wiki/2D-Kollisionserkennung#Kollision_zwischen_zwei_Strecken
bool testIntersect(const double pointX, const double pointY,
                          double wayX1, double wayY1, double wayX2, double wayY2)
{
    const double pointX1 = pointX,
                pointX2 = pointX-370,
                pointY1 = pointY,
                pointY2 = pointY; //point outside of the range of all possible points
    const double denom = (wayY2 - wayY1) * (pointX2 - pointX1) -
                        (wayX2 -  wayX1) * (pointY2 - pointY1);
    if (abs(denom) < 0.000001) return false;

    const double ua = ((wayX2 - wayX1) * (pointY1 - wayY1) -
                      (wayY2 - wayY1) * (pointX1 - wayX1)) / denom;
    const double ub = ((pointX2 - pointX1) * (pointY1 - wayY1) -
                      (pointY2 - pointY1) * (pointX1 - wayX1)) / denom;
    return ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1;
}
