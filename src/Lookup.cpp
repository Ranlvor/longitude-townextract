#include "Lookup.h"
#include "output.h"
Lookup::Lookup()
{
}

std::string Lookup::lookup(double latitude, double longitude){
    debug("Lookup::lookup(%f, %f)", latitude, longitude);
    std::vector<Borderrelation> canidates = db.getPossibleBorderrelations(latitude, longitude);
    for (std::vector<Borderrelation>::iterator it = canidates.begin() ; it != canidates.end(); ++it) {
        Borderrelation & canidate = *it;
        debug("checking border %s", canidate.name.c_str());
    }
    return canidates.begin()->name;
}
