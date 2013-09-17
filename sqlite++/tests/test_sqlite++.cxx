/**
@example test_sqlite++.cxx

This example illustrates the use of @ref sqlite

*/

#include "sqlite++/sqlite.hxx"
#include <iostream>

int main () {
  //![main]
  using namespace Sqlite;


  // Create a database connection
  Database database ("/tmp/db.sqlite");


  // Execute an SQL statement
  database.execute ("CREATE TABLE IF NOT EXISTS foo ("
                    "  id    INTEGER PRIMARY KEY,"
                    "  name  TEXT"
                    ")");

  {
    Transaction transaction(database);

    // Prepare an SQL statement with a placeholder, ...
    database.prepare ("INSERT INTO foo VALUES (NULL, ?)")
      .bind ("bar")  // bind it to a value, ...
      .step ();      // execute it
  }

  // Prepare an SQL statement
  Statement statement = database.prepare ("SELECT id, name FROM foo");

  // Fetch all returned rows
  while (statement.step() == SQLITE_ROW) {
    // Fetch column values from the row, ...
    int id;
    std::string name;
    statement >> id >> name;

    // and display them
    std::cerr << id << ": " << name << std::endl;
  }
  //![main]

  return 0;
}
