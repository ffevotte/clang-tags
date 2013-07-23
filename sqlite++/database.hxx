#pragma once

#include <string>
#include <memory>
#include <sqlite3.h>
#include <stdexcept>

namespace Sqlite {
  // Forward declarations
  class Statement;

  /** @addtogroup libsqlite
      @{
  */

  /** @brief Exception for SQLite errors
   */
  struct Error : public std::runtime_error {
    /** @brief Constructor
     *
     * @param what  error description
     */
    Error (const std::string & what)
      : std::runtime_error (what)
    {}
  };

  /** @brief Connection to an SQLite database
   */
  class Database {
  public:
    /** @brief Constructor
     *
     * Create a connection to the SQLite database stored in a given file.
     *
     * @param fileName  path to the SQLite database file
     * @throw Error
     */
    Database (const std::string & fileName) {
      sqlite3 *db;
      int ret = sqlite3_open (fileName.c_str(), &db);
      db_.reset (new Sqlite3_ (db));

      if (ret != SQLITE_OK) {
        throw Error (errMsg());
      }
    }

    /** @brief Execute a set of SQL statements
     *
     * @param sql  C-style string containing SQL statements
     *
     * @return @c SQLITE_OK if everything went well
     * @throw Error
     */
    int execute (char const *const sql);

    /** @brief Prepare execution of SQL statements
     *
     * Get a prepared Statement object, which can then be executed with bound
     * values.
     *
     * @param sql  C-style string containing SQL statements
     *
     * @return a prepared Statement object for the SQL code
     */
    Statement prepare (char const *const sql);

    /** @brief Retrieve the last SQLite error message
     *
     * @return a C-style string containing the error message
     */
    const char * errMsg () {
      return sqlite3_errmsg(raw());
    }

    /** @brief Retrieve the id of the last inserted row
     *
     * @return the row id, as an int
     */
    int lastInsertRowId () {
      return sqlite3_last_insert_rowid (raw());
    }

  private:
    sqlite3 * raw () { return db_->db_; }

    struct Sqlite3_ {
      sqlite3 *db_;
      Sqlite3_ (sqlite3 *db) : db_ (db) {}
      ~Sqlite3_ () { sqlite3_close (db_); }
    };
    std::shared_ptr<Sqlite3_> db_;

    friend class Statement;
  };

  /** @} */
}
