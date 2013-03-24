#pragma once

#include <string>
#include <memory>
#include <sqlite3.h>
#include <stdexcept>

namespace Sqlite {
  class Statement;

  struct Error : public std::runtime_error {
    Error (const std::string & what)
      : std::runtime_error (what)
    {}
  };

  class Database {
  public:
    Database (const std::string & fileName) {
      sqlite3 *db;
      int ret = sqlite3_open (fileName.c_str(), &db);
      db_.reset (new Sqlite3_ (db));

      if (ret != SQLITE_OK) {
        throw Error (errMsg());
      }
    }

    int execute (char const *const sql);

    Statement prepare (char const *const sql);

    const char * errMsg () {
      return sqlite3_errmsg(raw());
    }

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
}
