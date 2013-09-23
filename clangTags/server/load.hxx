#pragma once

#include "request/request.hxx"

#include "clangTags/storage.hxx"
#include "clangTags/indexer/indexer.hxx"

#include <string>
#include <iostream>

namespace ClangTags {
namespace Server {
/** @addtogroup clangTags
 *  @{
 */

/** @brief Read a compilation database
 *
 * This class provides a parser for the @c "load" command, which loads a
 * compilation database in JSON format, and stores all compilation commands.
 */
class Load : public Request::CommandParser {
public:
  /** @brief Constructor
   *
   * @param storage @ref Storage instance used to store the compilation commands
   * @param indexer @ref Indexer instance to be notified after the compilation
   *                database has been loaded.
   */
  Load (Storage & storage,
        Indexer::Indexer & indexer);
  ~Load ();

  void defaults ();

  /** @brief Read the compilation database.
   *
   * After all compilation commands have been stored, the Indexer is notified
   * that it should update the source files index.
   *
   * @param cout  output stream
   */
  void run (std::ostream & cout);

private:
  struct Args {
    std::string fileName;
  };
  Args args_;

  Storage & storage_;
  Indexer::Indexer & indexer_;
  char* cwd_;
};
/** @} */
}
}
