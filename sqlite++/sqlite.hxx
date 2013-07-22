#pragma once
#include "database.hxx"
#include "statement.hxx"

/** @namespace Sqlite

@brief Wrapper around the SQLite C API

Here is a brief example:

@code
#include "sqlite++/sqlite.hxx"
using namespace Sqlite;
Database database ("db.sqlite");

database.execute ("CREATE TABLE IF NOT EXISTS foo ("
                  "  id    INTEGER PRIMARY KEY,"
                  "  name  TEXT"
                  ")");

database.prepare ("INSERT INTO foo VALUES (NULL, ?)")
  .bind ("bar")
  .step ();

Statement statement = db.prepare ("SELECT id FROM foo WHERE name = ?")
  .bind ("bar");

while (statement.step() == SQLITE_ROW) {
  int id;
  statement >> id;
  std::cerr << "row id: " << id;
}
@endcode
*/
