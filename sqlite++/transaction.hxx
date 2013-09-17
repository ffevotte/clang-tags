#pragma once

namespace Sqlite {
  class Database;

  /** @addtogroup sqlite
      @{
  */

  /** @brief SQL transaction
   *
   * The transaction is automatically ended when the object is destroyed.
   */
  class Transaction {
  public:
    /** @brief Constructor
     *
     * Begin a transaction on a given SQLite database connection.
     *
     * @param db SQLite database connection.
     * @throw Error
     */
    Transaction (Database & db);

    /** @brief Destructor
     *
     * End the transaction.
     */
    ~Transaction ();

  private:
    Database & db_;
  };

  /** @} */
}
