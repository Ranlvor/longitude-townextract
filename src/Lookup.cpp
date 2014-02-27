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
                if(testIntersect(latitude, longitude, previous.latitude, previous.longitude, point.latitude, point.longitude)){
                    intersects++;
                }
            }
            previous = point;
        }
    }
    return (intersects % 2) == 1;
}

bool Lookup::testIntersect(double pointX, double pointY, double wayX1, double wayY1, double wayX2, double wayY2){
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
    if(intersectXPosition < pointX) //we only count matches right of the point, not left of the point, because our calculations made results for infinite lines but we have a searchray
        return true;
    else
        return false;
}
