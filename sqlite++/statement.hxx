#pragma once

#include "database.hxx"

namespace Sqlite {
  class Statement {
  public:
    Statement (Sqlite::Database & db, char const *const sql)
      : db_ (db),
        bindI_ (1),
        colI_ (0)
    {
      sqlite3_stmt *stmt;
      int ret = sqlite3_prepare_v2 (db_.raw(), sql, -1, &stmt, NULL);
      stmt_.reset (new Statement_ (stmt));

      if (ret != SQLITE_OK) {
        throw Error (db.errMsg());
      }
    }

    Statement & bind (const std::string s) {
      return bind_ (sqlite3_bind_text (raw(), bindI_, s.c_str(), -1, NULL));
    }

    Statement & bind (int i) {
      return bind_ (sqlite3_bind_int (raw(), bindI_, i));
    }

    Statement & operator>> (int & i) {
      i = sqlite3_column_int (raw(), colI_);
      ++colI_;
      return *this;
    }

    Statement & operator>> (std::string & s) {
      s = reinterpret_cast<char const *> (sqlite3_column_text (raw(), colI_));
      ++colI_;
      return *this;
    }

    int step () {
      colI_ = 0;
      int ret = sqlite3_step (raw());
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
}
