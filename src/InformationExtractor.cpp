#include "InformationExtractor.h"
#include "output.h"

InformationExtractor::InformationExtractor()
{
}

void inline InformationExtractor::primBlockCallbackPass1(OSMPBF::PrimitiveBlock primblock){
    OSMPBF::StringTable stringtable = primblock.stringtable();
    for(int i = 0, l = primblock.primitivegroup_size(); i < l; i++) {
        // one PrimitiveGroup from the the Block
        OSMPBF::PrimitiveGroup pg = primblock.primitivegroup(i);
        if(pg.relations_size() > 0) {
            debug("  Found relations");

            unsigned int adminLevelNumber = 0;
            unsigned int nameNumber = 0;
            for(int j = 0, m = stringtable.s_size(); j < m; j++) {
                std::string current = stringtable.s(j);
                if(current == "admin_level") {
                    adminLevelNumber = j;
                    debug("    adminLevel = %d", adminLevelNumber);
                } else if (current == "name") {
                    nameNumber = j;
                    debug("    name = %d", nameNumber);
                }
            }

            if(adminLevelNumber == 0)
                return; //if adminlevel is not found in the stringtable, it's impossible for a border to apear in this block

            for(int j = 0, m = pg.relations_size(); j < m; j++) {
                OSMPBF::Relation r = pg.relations(j);
                bool isBorder = false;

                for(int k = 0, n = r.keys_size(); k < n; k++) {
                    if (r.keys(k) == adminLevelNumber){
                        isBorder = true;
                        break;
                    }
                }

                if(isBorder){
                    std::string name;
                    std::string adminlevelstr;
                    for(int k = 0, n = r.keys_size(); k < n; k++) {
                        if (r.keys(k) == adminLevelNumber){
                            isBorder = true;
                            adminlevelstr = stringtable.s(r.vals(k));
                        } else if (r.keys(k) == nameNumber){
                            name = stringtable.s(r.vals(k));
                        }
                    }
                    debug("    found border (level %s) %s", adminlevelstr.c_str(), name.c_str());
                }
            }
            //for(int j = 0; )
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
}

void InformationExtractor::nextPass(){
    pass++;
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

}
