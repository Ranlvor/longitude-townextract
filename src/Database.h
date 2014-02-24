#ifndef DATABASE_H
#define DATABASE_H
#include<string>
#include <sqlite3.h>
class Database
{
public:
    Database();
    ~Database();

    void beginTransaction();
    void commitTransaction();

    void insertBorderRelation(long long int relationid, const std::string & name, int adminlevel);
    void insertRelationWay(long long int relationid, long long int wayid, int type);

private:
    sqlite3 *db;
    sqlite3_stmt *stmtBeginTransaction;
    sqlite3_stmt *stmtCommitTransaction;
    sqlite3_stmt *stmtInsertBorderRelation;
    sqlite3_stmt *stmtInsertRelationWay;
};

#endif // DATABASE_H
