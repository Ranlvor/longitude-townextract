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
    //sqlite3_stmt *stmtInsertBorderRelation;
    //sqlite3_stmt *stmtInsertRelationWay;
}
Database::~Database() {
    sqlite3_finalize(stmtBeginTransaction);
    sqlite3_finalize(stmtCommitTransaction);
    sqlite3_finalize(stmtInsertBorderRelation);
    sqlite3_finalize(stmtInsertRelationWay);
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
    debug("Database::insertBorderRelation(%d, %s, %d", relationid, name.c_str(), adminlevel);
    sqlite3_bind_int64(stmtInsertBorderRelation, 1, relationid);
    sqlite3_bind_text(stmtInsertBorderRelation, 2, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmtInsertBorderRelation, 3, adminlevel);
    sqlite3_step(stmtInsertBorderRelation);
    sqlite3_reset(stmtInsertBorderRelation);
}

void Database::insertRelationWay(long long int relationid, long long int wayid) {

}
