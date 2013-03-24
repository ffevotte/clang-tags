#include "database.hxx"
#include "statement.hxx"

namespace Sqlite {
  int Database::execute (char const *const sql) {
    Statement stmt = prepare (sql);
    return stmt.step();
  }

  Statement Database::prepare (char const *const sql) {
    return Statement (*this, sql);
  }
}
