#pragma once

#include "database.hxx"
#include <iostream>
#include <sys/poll.h>

namespace Sqlite {
  /** @addtogroup sqlite
      @{
  */

  /** @brief Prepared SQL statement
   *
   * Objects of this type represent prepared SQL statements destined to be
   * executed on an SQLite database.
   *
   * Prepared SQL statements can have placeholders (written as question marks @c
   * '?' in the SQL request) which can be bound to real values (see @ref bind)
   * before being executed (see @ref step).
   *
   * Values can be extracted from the query results using operator>>().
   */
  class Statement {
  public:
    /** @brief Constructor
     *
     * Create a prepared statement for a SQL request on a given SQLite database
     * connection.
     *
     * @param db   SQLite Database connection
     * @param sql  SQL request
     * @throw Error
     */
    Statement (Sqlite::Database & db, char const *const sql)
      : db_ (db),
        bindI_ (1),
        colI_ (0)
    {
      sqlite3_stmt *stmt;
      int ret = sqlite3_prepare_v2 (db_.raw(), sql, -1, &stmt, NULL);
      stmt_.reset (new Statement_ (stmt));

      if (ret != SQLITE_OK) {
        throw Error (std::string(sql) + "\n"
                     + std::string(db.errMsg()));
      }
    }

    /** @brief Bind a placeholder to a value
     *
     * This method should be used for all value types except @c int. This method
     * returns the Statement object itself, allowing chains of calls.
     *
     * @param s  string representing the value to be bound
     *
     * @return the Statement object itself
     */
    Statement & bind (const std::string & s) {
      return bind_ (sqlite3_bind_text (raw(), bindI_, s.c_str(), s.size(), NULL));
    }

    /** @brief Bind a placeholder to a value
     *
     * This method should be used for @c int values. This method returns the
     * Statement object itself, allowing chains of calls.
     *
     * @param i  integer value to be bound
     *
     * @return the Statement object itself
     */
    Statement & bind (int i) {
      return bind_ (sqlite3_bind_int (raw(), bindI_, i));
    }

    /** @brief Extract an @c int value from the current result row
     *
     * This method returns the Statement object itself, allowing chains of calls.
     *
     * @param i  variable where the value will be stored
     *
     * @return the Statement object itself
     */
    Statement & operator>> (int & i) {
      i = sqlite3_column_int (raw(), colI_);
      ++colI_;
      return *this;
    }

    /** @brief Extract a value from the current result row
     *
     * This method should be called for all value types except @c int. It
     * returns the Statement object itself, allowing chains of calls.
     *
     * @param s  variable where the value will be stored
     *
     * @return the Statement object itself
     */
    Statement & operator>> (std::string & s) {
      s = reinterpret_cast<char const *> (sqlite3_column_text (raw(), colI_));
      ++colI_;
      return *this;
    }

    /** @brief Execute the statement or fetch the next result row
     *
     * Execute the SQL statement, after all placeholders have been bound. If no
     * results are produced by the statement, return @c SQLITE_OK. Otherwise,
     * fetch the first result row and return @c SQLITE_ROW.
     *
     * Fetch a new row and return @c SQLITE_ROW for all subsequent calls. When
     * no more rows are available, return @c SQLITE_DONE.
     *
     * Values can be extracted from the current result row using operator>>().
     *
     * @return @c SQLITE_OK, @c SQLITE_ROW or @c SQLITE_DONE
     * @throw Error
     */
    int step (bool retry = true) {
      colI_ = 0;
      int ret = sqlite3_step (raw());

      // Retry statement if requested
      if (retry && ret == SQLITE_BUSY) {
        for ( ; ; ) {
          // sleep for 0.1 s.
          poll (NULL, 0, 1000);

          std::cerr << "Database busy... retrying " << std::endl
                    << sqlite3_sql (raw()) << std::endl;
          ret = sqlite3_step (raw());
          if (ret != SQLITE_BUSY) break;
        }
      }

      if (ret == SQLITE_ERROR) {
        throw Error (db_.errMsg());
      }

      return ret;
    }

  private:
    Statement & bind_ (int ret) {
      if (ret != SQLITE_OK) {
        throw Error (db_.errMsg());
      }

      ++bindI_;
      return *this;
    }

    sqlite3_stmt * raw () { return stmt_->stmt_; }

    struct Statement_ {
      sqlite3_stmt *stmt_;
      Statement_ (sqlite3_stmt *stmt) : stmt_(stmt) {}
      ~Statement_ () { sqlite3_finalize (stmt_); }
    };

    Database & db_;
    int bindI_;
    int colI_;
    std::shared_ptr<Statement_> stmt_;
  };

  /** @} */
}
