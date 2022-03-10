#include "db.h"
#include "err.h"
#include <sqlite3.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int
returnID(void *, int, char **, char **);
static int
doNothing(void *, int, char **, char **);
static int
returnPhilosophy(void *, int, char **, char **);

sqlite3 *DB_open(const char *dbName)
{
    sqlite3 *db;
    if (sqlite3_open(dbName, &db) != SQLITE_OK)
        errExit("sqlite3_open");
    return db;
}
void DB_close(sqlite3 *db)
{
    if (sqlite3_close(db) != SQLITE_OK)
        errExit("sqlite3_close");
}

void DB_insert(sqlite3 *db, const char *sql, int (*callback)(void *, int, char **, char **), void *data)
{
    DB_exec(db, sql, callback, data);
}
void DB_select(sqlite3 *db, const char *sql, int (*callback)(void *, int, char **, char **), void *data)
{
    DB_exec(db, sql, callback, data);
}
void DB_delete(sqlite3 *db, const char *sql, int (*callback)(void *, int, char **, char **), void *data)
{
    DB_exec(db, sql, callback, data);
}
void DB_update(sqlite3 *db, const char *sql, int (*callback)(void *, int, char **, char **), void *data)
{
    DB_exec(db, sql, callback, data);
}
void DB_exec(sqlite3 *db, const char *sql, int (*callback)(void *, int, char **, char **), void *data)
{
    char *errMsg;
    if (sqlite3_exec(db, sql, callback, data, &errMsg) != SQLITE_OK)
        errExit("%s", errMsg);
    sqlite3_free(errMsg);
}

size_t DB_getMaxID(sqlite3 *db, const char *tableName)
{
    char sql[szSql];
    size_t maxID;
    snprintf(sql, sizeof(sql), "SELECT MAX(ID) FROM %s;", tableName);
    DB_select(db, sql, returnID, &maxID);
    return maxID;
}
void DB_insertPhilosophy(sqlite3 *db, const char *tableName, const char *author, const char *text)
{
    char sql[szSql];
    snprintf(sql, sizeof(sql), "INSERT INTO %s (ID, AUTHOR, TEXT)\n"
                               "VALUES (%ld, \"%s\", \"%s\");",
             tableName,
             DB_getMaxID(db, tableName) + 1, author, text);
    DB_insert(db, sql, doNothing, NULL);
}
void DB_getPhilosophy(size_t id, sqlite3 *db, const char *tableName, Philosophy *philosophy)
{
    char sql[szSql];
    snprintf(sql, sizeof(sql), "SELECT * FROM %s WHERE ID=%ld;",
             tableName, id);
    DB_select(db, sql, returnPhilosophy, philosophy);
}
static int returnPhilosophy(void *data, int argc, char **argv, char **azColumns)
{
    Philosophy *philosophy = data;
    philosophy->id = atoi(argv[0]);
    strcpy(philosophy->text, argv[1]);
    strcpy(philosophy->author, argv[2]);
    return 0;
}
static int returnID(void *data, int argc, char **argv, char **nouse4)
{
    if (argv[0] == NULL)
        *(size_t *)data = -1;
    else
        *(size_t *)data = atoi(argv[0]);
    return 0;
}
static int doNothing(void *nouse, int n2, char **n3, char **n4)
{
    return 0;
}