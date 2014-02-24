#include "InformationExtractor.h"
#include "output.h"
#include <string>
#include <sstream>
InformationExtractor::InformationExtractor()
{
}

template <typename T>
T StringToNumber ( const std::string &Text )//Text not by const reference so that the function can be used with a
{                               //character array as argument
    std::stringstream ss(Text);
    T result;
    return ss >> result ? result : 0;
}

void inline InformationExtractor::primBlockCallbackPass1(OSMPBF::PrimitiveBlock primblock){
    OSMPBF::StringTable stringtable = primblock.stringtable();
    for(int i = 0, l = primblock.primitivegroup_size(); i < l; i++) {
        // one PrimitiveGroup from the the Block
        OSMPBF::PrimitiveGroup pg = primblock.primitivegroup(i);
        if(pg.relations_size() > 0) {
            debug("  Found relations");

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
                    debug("    adminLevel = %d", adminLevelNumber);
                } else if (current == "name") {
                    nameNumber = j;
                    debug("    name = %d", nameNumber);
                } else if (current == "administrative") {
                    administrativeNumber = j;
                    debug("    administrative = %d", administrativeNumber);
                } else if (current == "boundary") {
                    boundaryNumber = j;
                    debug("    boundary = %d", boundaryNumber);
                } else if (current == "inner") {
                    innerNumber = j;
                    debug("    inner = %d", innerNumber);
                } else if (current == "outer") {
                    outerNumber = j;
                    debug("    outer = %d", outerNumber);
                } else if (current == "enclave") {
                    enclaveNumber = j;
                    debug("    enclave = %d", enclaveNumber);
                } else if (current == "exclave") {
                    exclaveNumber = j;
                    debug("    exclave = %d", exclaveNumber);
                } else if (current == "") {
                    emptyStringNumber = j;
                    debug("    (empty string) = %d", emptyStringNumber);
                }
            }

            if(adminLevelNumber == 0 || administrativeNumber == 0 || boundaryNumber == 0)
                return; //if adminlevel is not found in the stringtable, it's impossible for a border to apear in this block

            for(int j = 0, m = pg.relations_size(); j < m; j++) {
                OSMPBF::Relation r = pg.relations(j);
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
                    int adminlevel;
                    long long int id = r.id();
                    for(int k = 0, n = r.keys_size(); k < n; k++) {
                        if (r.keys(k) == adminLevelNumber){
                            adminlevel = StringToNumber<int>( stringtable.s( r.vals(k) ) );
                        } else if (r.keys(k) == nameNumber){
                            name = stringtable.s(r.vals(k));
                        }
                    }

                    debug("    found border (level %d, id %d) %s", adminlevel, id, name.c_str());
                    db.insertBorderRelation(id, name, adminlevel);

                    long long int memid = 0;
                    for(int a = 0, b = r.memids_size(); a < b; a++) {
                        memid += r.memids(a);
                        if(r.types(a) == r.WAY) {
                            int role = -1;
                            unsigned int roleStringId = r.roles_sid(a);
                            if(roleStringId == 0 || roleStringId == outerNumber || roleStringId == exclaveNumber || roleStringId == emptyStringNumber)
                                role = 1;
                            else if (roleStringId == innerNumber || roleStringId == enclaveNumber)
                                role = 0;

                            if(role < 0) {
                                warn("      found way (%d) of unknown role (%s)", memid, stringtable.s(roleStringId).c_str());
                            } else {
                                db.insertRelationWay(id, memid, role);
                            }
                        }
                    }
                }
            }
        }
    }
}

void inline InformationExtractor::primBlockCallbackPass2(OSMPBF::PrimitiveBlock primblock){

}

void inline InformationExtractor::primBlockCallbackPass3(OSMPBF::PrimitiveBlock primblock){

}

void InformationExtractor::init(){
    debug("InformationExtractor::init prepare for pass 1");
    pass = 1;
    db.beginTransaction();
}

void InformationExtractor::nextPass(){
    db.commitTransaction();
    pass++;
    db.beginTransaction();
    info("\n\nSwitching to pass %d", pass);
}

void InformationExtractor::primBlockCallback(OSMPBF::PrimitiveBlock primblock){
    debug("InformationExtractor::primBlockCallback");
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
    db.commitTransaction();
}
