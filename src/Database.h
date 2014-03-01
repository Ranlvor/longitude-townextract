#ifndef DATABASE_H
#define DATABASE_H
#include<string>
#include<vector>
#include <sqlite3.h>

struct Borderrelation {
    long long int relationid;
    int adminlevel;
    std::string name;
};

struct Point {
    long long int id;
    double latitude;
    double longitude;
};

enum WayType{ invalid = -1, inner = 0, outer = 1 };
struct Way {
    long long int id;
    WayType type;
    std::vector<Point> points;
};

class Database
{
public:
    Database();
    ~Database();

    void beginTransaction();
    void commitTransaction();

    void insertBorderRelation(long long int relationid, const std::string & name, int adminlevel);
    void insertRelationWay(long long int relationid, long long int wayid, WayType type);
    void insertWayPoint(long long int wayid, long long int pointid, int order);
    void insertPoint(long long int pointid, double lat, double lon);

    void buildBoundingboxIndex();

    std::vector<Borderrelation> getPossibleBorderrelations(double lat, double lon);
    std::vector<Way> getBorderGeometry(long long int borderid);

    void minimizeMemoryUssage();

private:
    sqlite3 *db;
    sqlite3_stmt *stmtBeginTransaction;
    sqlite3_stmt *stmtCommitTransaction;
    sqlite3_stmt *stmtInsertBorderRelation;
    sqlite3_stmt *stmtInsertRelationWay;
    sqlite3_stmt *stmtInsertWayPoint;
    sqlite3_stmt *stmtInsertPoint;
    sqlite3_stmt *stmtGetPossibleBorderrelations;
    sqlite3_stmt *stmtGetBorderGeometry;

    void openDatabase();
    void closeDatabase();
};

#endif // DATABASE_H
