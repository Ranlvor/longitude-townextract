#ifndef INFORMATIONEXTRACTOR_H
#define INFORMATIONEXTRACTOR_H
#include <osmpbf/osmpbf.h>
#include "Database.h"
#include <unordered_set>
class InformationExtractor
{
public:
    InformationExtractor();

    void init();
    void primBlockCallback(const OSMPBF::PrimitiveBlock& primblock);
    void nextPass();
    void finish();

private:
    int pass;
    Database db;
    std::unordered_set<long long int> interestingWays;
    std::unordered_set<long long int> interestingPoints;
    void primBlockCallbackPass1(const OSMPBF::PrimitiveBlock& primblock);
    void primBlockCallbackPass2(const OSMPBF::PrimitiveBlock& primblock);
    void primBlockCallbackPass3(const OSMPBF::PrimitiveBlock& primblock);
};

#endif // INFORMATIONEXTRACTOR_H
