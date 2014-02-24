#ifndef INFORMATIONEXTRACTOR_H
#define INFORMATIONEXTRACTOR_H
#include <osmpbf/osmpbf.h>

class InformationExtractor
{
public:
    InformationExtractor();

    void init();
    void primBlockCallback(OSMPBF::PrimitiveBlock primblock);
    void nextPass();
    void finish();

private:
    int pass;
};

#endif // INFORMATIONEXTRACTOR_H
