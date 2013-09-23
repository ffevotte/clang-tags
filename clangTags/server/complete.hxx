#pragma once

#include "request/request.hxx"
#include "clangTags/cache.hxx"

#include <string>

namespace ClangTags {
namespace Server {
/** @addtogroup clangTags
 *  @{
 */

/** @brief Propose completions for the source code at point
 */
class Complete : public Request::CommandParser {
public:
  /** @brief Constructor
   *
   * @param storage  @ref Storage instance used for the compilation database
   * @param cache    translation units @ref Cache
   */
  Complete (Storage & storage,
            Cache & cache);

  void defaults ();
  void run (std::ostream & cout);

private:
  struct Args {
    std::string fileName;
    int         line;
    int         column;
  };
  Args args_;

  Storage & storage_;
  Cache & cache_;
};
/** @} */
}
}
