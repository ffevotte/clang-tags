#include "transaction.hxx"

#include "database.hxx"

namespace Sqlite {
  Transaction::Transaction (Database & db)
    : db_(db)
  {
    db_.execute("BEGIN IMMEDIATE TRANSACTION");
  }

  Transaction::~Transaction () {
    db_.execute("END TRANSACTION");
  }
}
