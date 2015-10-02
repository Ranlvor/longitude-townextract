#include "InformationExtractor.h"
#include "Output.h"
#include "stringToNumber.h"
#include <string>
InformationExtractor::InformationExtractor()
{
}

void inline InformationExtractor::primBlockCallbackPass1(const OSMPBF::PrimitiveBlock& primblock){
    const OSMPBF::StringTable& stringtable = primblock.stringtable();
    for(int i = 0, l = primblock.primitivegroup_size(); i < l; i++) {
        // one PrimitiveGroup from the the Block
        const OSMPBF::PrimitiveGroup& pg = primblock.primitivegroup(i);
        if(pg.relations_size() > 0) {
#ifdef DEBUTOUGPUT
            Output::debug("  Found relations");
#endif //DEBUTOUTPUT
            unsigned int adminLevelNumber = 0;
            unsigned int administrativeNumber = 0;
            unsigned int boundaryNumber = 0;
            unsigned int nameNumber = 0;
            unsigned int innerNumber = 0;
            unsigned int outerNumber = 0;
            unsigned int enclaveNumber = 0;
            unsigned int exclaveNumber = 0;
            unsigned int emptyStringNumber = 0;
            for(int j = 0, m = stringtable.s_size(); j < m; j++) {
                std::string current = stringtable.s(j);
                if(current == "admin_level") {
                    adminLevelNumber = j;
#ifdef DEBUTOUGPUT
                    Output::debug("    adminLevel = %d", adminLevelNumber);
#endif //DEBUTOUTPUT
                } else if (current == "name") {
                    nameNumber = j;
#ifdef DEBUTOUGPUT
                    Output::debug("    name = %d", nameNumber);
#endif //DEBUTOUTPUT
                } else if (current == "administrative") {
                    administrativeNumber = j;
#ifdef DEBUTOUGPUT
                    Output::debug("    administrative = %d", administrativeNumber);
#endif //DEBUTOUTPUT
                } else if (current == "boundary") {
                    boundaryNumber = j;
#ifdef DEBUTOUGPUT
                    Output::debug("    boundary = %d", boundaryNumber);
#endif //DEBUTOUTPUT
                } else if (current == "inner") {
                    innerNumber = j;
#ifdef DEBUTOUGPUT
                    Output::debug("    inner = %d", innerNumber);
#endif //DEBUTOUTPUT
                } else if (current == "outer") {
                    outerNumber = j;
#ifdef DEBUTOUGPUT
                    Output::debug("    outer = %d", outerNumber);
#endif //DEBUTOUTPUT
                } else if (current == "enclave") {
                    enclaveNumber = j;
#ifdef DEBUTOUGPUT
                    Output::debug("    enclave = %d", enclaveNumber);
#endif //DEBUTOUTPUT
                } else if (current == "exclave") {
                    exclaveNumber = j;
#ifdef DEBUTOUGPUT
                    Output::debug("    exclave = %d", exclaveNumber);
#endif //DEBUTOUTPUT
                } else if (current == "") {
                    emptyStringNumber = j;
#ifdef DEBUTOUGPUT
                    Output::debug("    (empty string) = %d", emptyStringNumber);
#endif //DEBUTOUTPUT
                }
            }

            if(adminLevelNumber == 0 || administrativeNumber == 0 || boundaryNumber == 0)
                return; //if adminlevel is not found in the stringtable, it's impossible for a border to apear in this block

            for(int j = 0, m = pg.relations_size(); j < m; j++) {
                const OSMPBF::Relation& r = pg.relations(j);
                bool hasAdminLevel = false;
                bool hasBoundaryAdministrative = false;

                for(int k = 0, n = r.keys_size(); k < n; k++) {
                    if (r.keys(k) == adminLevelNumber){
                        hasAdminLevel = true;
                    } else if (r.keys(k) == boundaryNumber){
                        if(r.vals(k) == administrativeNumber)
                            hasBoundaryAdministrative = true;
                    }
                }

                if(hasAdminLevel && hasBoundaryAdministrative){
                    std::string name;
                    int adminlevel = 0;
                    long long int id = r.id();
                    for(int k = 0, n = r.keys_size(); k < n; k++) {
                        if (r.keys(k) == adminLevelNumber){
                            adminlevel = StringToNumber<int>( stringtable.s( r.vals(k) ) );
                        } else if (r.keys(k) == nameNumber){
                            name = stringtable.s(r.vals(k));
                        }
                    }
#ifdef DEBUTOUGPUT
                    Output::debug("    found border (level %d, id %lld) %s", adminlevel, id, name.c_str());
#endif //DEBUTOUTPUT
                    db.insertBorderRelation(id, name, adminlevel);

                    long long int memid = 0;
                    for(int a = 0, b = r.memids_size(); a < b; a++) {
                        memid += r.memids(a);
                        if(r.types(a) == r.WAY) {
                            WayType role = invalid;
                            unsigned int roleStringId = r.roles_sid(a);
                            if(roleStringId == 0 || roleStringId == outerNumber || roleStringId == exclaveNumber || roleStringId == emptyStringNumber)
                                role = outer;
                            else if (roleStringId == innerNumber || roleStringId == enclaveNumber)
                                role = inner;

                            if(role == invalid) {
                                Output::warn("      found way (%lld) of unknown role (%s)", memid, stringtable.s(roleStringId).c_str());
                            } else {
                                db.insertRelationWay(id, memid, role);
                                interestingWays.insert(memid);
                            }
                        }
                    }
                }
            }
        }
    }
}

void inline InformationExtractor::primBlockCallbackPass2(const OSMPBF::PrimitiveBlock& primblock){
    for(int i = 0, l = primblock.primitivegroup_size(); i < l; i++) {
        const OSMPBF::PrimitiveGroup& pg = primblock.primitivegroup(i);
        if(pg.ways_size() > 0) {
            for(int j = 0, l = pg.ways_size(); j < l; j++) {
                const OSMPBF::Way& way = pg.ways(j);
                long long int wayid = way.id();
                if(interestingWays.count(wayid) != 0) {

#ifdef DEBUTOUGPUT
                    Output::debug("  found way %lld", wayid);
#endif //DEBUTOUTPUT

                    long long int pointid = 0;
                    for(int k = 0, l = way.refs_size(); k < l; k++) {
                        pointid += way.refs(k);
                        db.insertWayPoint(wayid, pointid, k);
                        interestingPoints.insert(pointid);
                    }
                }
            }
        }
    }
}

void inline InformationExtractor::primBlockCallbackPass3(const OSMPBF::PrimitiveBlock& primblock){
    long long int lat_offset = primblock.lat_offset();
    long long int lon_offset = primblock.lon_offset();
    int granularity = primblock.granularity();
    for(int i = 0, l = primblock.primitivegroup_size(); i < l; i++) {
        const OSMPBF::PrimitiveGroup& pg = primblock.primitivegroup(i);
        if(pg.nodes_size() > 0) {
            Output::err("has non-dense nodes and non-dense nodes are not implemented");
        }

        // tell about dense nodes
        if(pg.has_dense()) {
            const OSMPBF::DenseNodes& nodes = pg.dense();
            long long int id = 0;
            long long int latitude = 0;
            long long int longitude = 0;
            for(int j = 0, l = nodes.id_size(); j < l; j++) {
                id += nodes.id(j);
                latitude += nodes.lat(j);
                longitude += nodes.lon(j);
                if(interestingPoints.count(id) != 0) {
                    double databaseLat, databaseLon;

                    databaseLat = (lat_offset + ((double)granularity * (double)latitude))/OSMPBF::lonlat_resolution;
                    databaseLon = (lon_offset + ((double)granularity * (double)longitude))/OSMPBF::lonlat_resolution;
                    db.insertPoint(id, databaseLat, databaseLon);
#ifdef DEBUTOUGPUT
                    Output::debug("  point: %lld, %d (%F), %d (%F)",id, latitude, databaseLat, longitude, databaseLon);
#endif //DEBUTOUTPUT
                }
            }
        }
    }
}

void InformationExtractor::init(){
    Output::debug("InformationExtractor::init prepare for pass 1");
    pass = 1;
    db.beginTransaction();
}

void InformationExtractor::nextPass(){
    pass++;
    Output::info("");
    Output::info("");
    Output::info("Switching to pass %d", pass);
    if(pass == 3) {
        interestingWays = std::unordered_set<long long int> (); //We do not need this anymore
    }
    db.commitTransaction();
    db.beginTransaction();
}

void InformationExtractor::primBlockCallback(const OSMPBF::PrimitiveBlock& primblock){
#ifdef DEBUTOUGPUT
    Output::debug("InformationExtractor::primBlockCallback");
#endif //DEBUTOUTPUT

    switch (pass){
        case 1:
            return primBlockCallbackPass1(primblock);
            break;

        case 2:
            return primBlockCallbackPass2(primblock);
            break;

        case 3:
            return primBlockCallbackPass3(primblock);
            break;
    }
}

void InformationExtractor::finish(){
    interestingPoints = std::unordered_set<long long int> (); //We do not need this anymore
    db.commitTransaction();
    db.minimizeMemoryUssage();
    Output::info("");
    Output::info("Building boundingbox-index");
    db.buildBoundingboxIndex();
}
