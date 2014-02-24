#include "Database.h"
#include "output.h"

Database::Database() {
    sqlite3_open("./db/borderdatabase.db", &db);

    sqlite3_prepare_v2(db, "BEGIN TRANSACTION;", -1, &stmtBeginTransaction, 0);
    sqlite3_prepare_v2(db, "COMMIT;", -1, &stmtCommitTransaction, 0);
    sqlite3_prepare_v2(db,
                       "INSERT OR REPLACE INTO borderrelations (relationid, name, adminlevel)"
                       "VALUES (?1, ?2, ?3)",
                       -1, &stmtInsertBorderRelation, 0);
    sqlite3_prepare_v2(db,
                       "INSERT OR REPLACE INTO relationway (relation, way, type)"
                       "VALUES (?1, ?2, ?3)",
                       -1, &stmtInsertRelationWay, 0);
    sqlite3_prepare_v2(db,
                       "INSERT OR REPLACE INTO waypoint (way, point)"
                       "VALUES (?1, ?2)",
                       -1, &stmtInsertWayPoint, 0);
    sqlite3_prepare_v2(db,
                       "INSERT OR REPLACE INTO point (id, lat, lon)"
                       "VALUES (?1, ?2, ?3)",
                       -1, &stmtInsertPoint, 0);
}
Database::~Database() {
    sqlite3_finalize(stmtBeginTransaction);
    sqlite3_finalize(stmtCommitTransaction);
    sqlite3_finalize(stmtInsertBorderRelation);
    sqlite3_finalize(stmtInsertRelationWay);
    sqlite3_finalize(stmtInsertWayPoint);
    sqlite3_finalize(stmtInsertPoint);
    sqlite3_close_v2(db);
}

void Database::beginTransaction() {
    sqlite3_step(stmtBeginTransaction);
    sqlite3_reset(stmtBeginTransaction);
}

void Database::commitTransaction() {
    sqlite3_step(stmtCommitTransaction);
    sqlite3_reset(stmtCommitTransaction);
}

void Database::insertBorderRelation(long long int relationid, const std::string & name, int adminlevel) {
    //debug("Database::insertBorderRelation(%d, %s, %d", relationid, name.c_str(), adminlevel);
    sqlite3_bind_int64(stmtInsertBorderRelation, 1, relationid);
    sqlite3_bind_text(stmtInsertBorderRelation, 2, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmtInsertBorderRelation, 3, adminlevel);
    sqlite3_step(stmtInsertBorderRelation);
    sqlite3_reset(stmtInsertBorderRelation);
}

//type: 0 = inner, 1 = outer
void Database::insertRelationWay(long long int relationid, long long int wayid, int type) {
    sqlite3_bind_int64(stmtInsertRelationWay, 1, relationid);
    sqlite3_bind_int64(stmtInsertRelationWay, 2, wayid);
    sqlite3_bind_int(stmtInsertRelationWay, 3, type);
    sqlite3_step(stmtInsertRelationWay);
    sqlite3_reset(stmtInsertRelationWay);
}


void Database::insertWayPoint(long long int wayid, long long int pointid) {
    sqlite3_bind_int64(stmtInsertWayPoint, 1, wayid);
    sqlite3_bind_int64(stmtInsertWayPoint, 2, pointid);
    sqlite3_step(stmtInsertWayPoint);
    sqlite3_reset(stmtInsertWayPoint);
}

void Database::insertPoint(long long int pointid, double lat, double lon) {
    sqlite3_bind_int64(stmtInsertPoint, 1, pointid);
    sqlite3_bind_double(stmtInsertPoint, 2, lat);
    sqlite3_bind_double(stmtInsertPoint, 3, lon);
    sqlite3_step(stmtInsertPoint);
    sqlite3_reset(stmtInsertPoint);
}
