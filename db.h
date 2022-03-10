#ifndef DB_H
#define DB_H

#define szSql 10000

#include <sqlite3.h>
#include <stddef.h>

#define szPhilosophyText 500
#define szAuthor         100
typedef struct Philosophy{
    char author[szAuthor];
    char text[szPhilosophyText];
    size_t id;
}Philosophy;

sqlite3 *DB_open(const char *dbName);
void DB_close(sqlite3 *db);
void DB_exec(sqlite3 *db, const char *sql, int (*callback)(void *, int, char **, char **), void *data);
void DB_insert(sqlite3 *db, const char *sql, int (*callback)(void *, int, char **, char **), void *data);
void DB_select(sqlite3 *db, const char *sql, int (*callback)(void *, int, char **, char **), void *data);
void DB_delete(sqlite3 *db, const char *sql, int (*callback)(void *, int, char **, char **), void *data);
void DB_update(sqlite3 *db, const char *sql, int (*callback)(void *, int, char **, char **), void *data);

size_t DB_getMaxID(sqlite3 *db, const char *tableName);
void DB_insertPhilosophy(sqlite3 *db, const char *tableName, const char *author, const char *text);
void DB_getPhilosophy(size_t id, sqlite3 *db, const char *tableName, Philosophy* philosophy);
#endif