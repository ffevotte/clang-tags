#pragma once

#include "request/request.hxx"
#include "clangTags/storage.hxx"

namespace ClangTags {
namespace Server {
/** @addtogroup clangTags
 *  @{
 */

/** @brief Find all references to a symbol
 *
 * This class provides a parser for the @c "grep" command, which looks in the
 * project's sources for all references to a given symbol.
 */
class Grep : public Request::CommandParser {
public:
  /** @brief Constructor
   *
   * @param storage @ref Storage instance where to look for indexed tags
   */
  Grep (Storage & storage);

  void defaults ();

  void run (std::ostream & cout);

private:
  struct Args {
    std::string usr;
  };
  Args args_;

  Storage & storage_;
};
/** @} */
}
}
