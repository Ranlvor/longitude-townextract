#include "InformationExtractor.h"
#include "output.h"

InformationExtractor::InformationExtractor()
{
}

void InformationExtractor::init(){
    debug("InformationExtractor::init prepare for pass 1");
    pass = 1;
}

void InformationExtractor::nextPass(){
    pass++;
    info("\n\nSwitching to pass %d", pass);
}

void InformationExtractor::primBlockCallback(OSMPBF::PrimitiveBlock /*primblock*/){
    debug("InformationExtractor::primBlockCallback");
}

void InformationExtractor::finish(){

}
