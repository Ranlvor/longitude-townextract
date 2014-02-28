#include "Database.h"
#include "output.h"

Database::Database() {
    sqlite3_open("./db/borderdatabase.db", &db);

    sqlite3_stmt *stmttmp;
    sqlite3_prepare_v2(db, "PRAGMA cache_size = -131072;", -1, &stmttmp, 0);
    sqlite3_step(stmttmp);
    sqlite3_finalize(stmttmp);

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
                       "INSERT OR REPLACE INTO waypoint (way, point, 'order')"
                       "VALUES (?1, ?2, ?3)",
                       -1, &stmtInsertWayPoint, 0);
    sqlite3_prepare_v2(db,
                       "INSERT OR REPLACE INTO point (id, lat, lon)"
                       "VALUES (?1, ?2, ?3)",
                       -1, &stmtInsertPoint, 0);

    sqlite3_prepare_v2(db,
                       "SELECT br.relationid, br.name, br.adminlevel "

                       "FROM boundingboxes AS bb "
                       "  LEFT JOIN borderrelations AS br ON (br.relationid = bb.relationid) "

                       "WHERE bb.minlat < ?1 "
                       "  AND bb.maxlat > ?1 "
                       "  AND bb.minlon < ?2 "
                       "  AND bb.maxlon > ?2 "
#ifndef DEBUGLOOKUP
                       "  AND br.adminlevel <= 8 "
#endif

                       "ORDER BY br.adminlevel DESC",
                       -1, &stmtGetPossibleBorderrelations, 0);


    sqlite3_prepare_v2(db,
                       "SELECT relationway.way AS way, relationway.type AS type, waypoint.point AS point, point.lat AS lat, point.lon AS lon "

                       "FROM relationway "
                       "  LEFT JOIN waypoint ON (relationway.way = waypoint.way) "
                       "  LEFT JOIN point ON (waypoint.point = point.id) "

                       "WHERE relationway.relation = ?1 "

                       "ORDER BY way ASC, waypoint.'order' ASC",
                       -1, &stmtGetBorderGeometry, 0);
}

Database::~Database() {
    sqlite3_finalize(stmtBeginTransaction);
    sqlite3_finalize(stmtCommitTransaction);
    sqlite3_finalize(stmtInsertBorderRelation);
    sqlite3_finalize(stmtInsertRelationWay);
    sqlite3_finalize(stmtInsertWayPoint);
    sqlite3_finalize(stmtInsertPoint);
    sqlite3_finalize(stmtGetPossibleBorderrelations);
    sqlite3_finalize(stmtGetBorderGeometry);
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
void Database::insertRelationWay(long long int relationid, long long int wayid, WayType type) {
    sqlite3_bind_int64(stmtInsertRelationWay, 1, relationid);
    sqlite3_bind_int64(stmtInsertRelationWay, 2, wayid);
    sqlite3_bind_int(stmtInsertRelationWay, 3, type);
    sqlite3_step(stmtInsertRelationWay);
    sqlite3_reset(stmtInsertRelationWay);
}


void Database::insertWayPoint(long long int wayid, long long int pointid, int order) {
    sqlite3_bind_int64(stmtInsertWayPoint, 1, wayid);
    sqlite3_bind_int64(stmtInsertWayPoint, 2, pointid);
    sqlite3_bind_int(stmtInsertWayPoint, 3, order);
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

void Database::buildBoundingboxIndex() {
    sqlite3_stmt *stmttmp;
    sqlite3_prepare_v2(db,
                       "INSERT INTO boundingboxes "
                       "SELECT rw.relation, MIN(p.lat) as minlat, MAX(p.lat) as maxlat, MIN(p.lon) as minlon, MAX(p.lon) as maxlon "

                       "FROM relationway as rw "
                       " LEFT JOIN waypoint as wp ON (rw.way = wp.way) "
                       " LEFT JOIN point as p ON (wp.point = p.id) "
                       "GROUP BY rw.relation; ",
                       -1, &stmttmp, 0);
    sqlite3_step(stmttmp);
    sqlite3_finalize(stmttmp);
}


std::vector<Borderrelation> Database::getPossibleBorderrelations(double lat, double lon){
    std::vector<Borderrelation> matches;
    sqlite3_bind_double(stmtGetPossibleBorderrelations, 1, lat);
    sqlite3_bind_double(stmtGetPossibleBorderrelations, 2, lon);
    while (sqlite3_step(stmtGetPossibleBorderrelations) == SQLITE_ROW){
        Borderrelation tmp;
        tmp.relationid = sqlite3_column_int64(stmtGetPossibleBorderrelations, 0);
        tmp.adminlevel = sqlite3_column_int(stmtGetPossibleBorderrelations, 2);
        tmp.name = (const char*) sqlite3_column_text(stmtGetPossibleBorderrelations, 1);
        matches.push_back(tmp);
    }

    sqlite3_reset(stmtGetPossibleBorderrelations);
    return matches;
}

std::vector<Way> Database::getBorderGeometry(long long int borderid){
    std::vector<Way> geometry;
    sqlite3_bind_int64(stmtGetBorderGeometry, 1, borderid);
    Way way;
    Point p;
    way.id = -1;
    while (sqlite3_step(stmtGetBorderGeometry) == SQLITE_ROW){
        long long int wayid = sqlite3_column_int64(stmtGetBorderGeometry, 0);
        WayType type = (WayType) sqlite3_column_int(stmtGetBorderGeometry, 1);
        long long int pointid = sqlite3_column_int64(stmtGetBorderGeometry, 2);
        double latitude = sqlite3_column_double(stmtGetBorderGeometry, 3);
        double longitude = sqlite3_column_double(stmtGetBorderGeometry, 4);
        if(way.id != wayid){
            if(way.id != -1) {
                geometry.push_back(way);
                way.points.clear();
            }
            way.id = wayid;
            way.type = type;
        }
        p.id = pointid;
        p.latitude = latitude;
        p.longitude = longitude;
        way.points.push_back(p);
    }
    geometry.push_back(way);

    sqlite3_reset(stmtGetBorderGeometry);
    return geometry;
}
