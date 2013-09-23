#pragma once

#include "request/request.hxx"

namespace ClangTags {
namespace Server {
/** @addtogroup clangTags
 *  @{
 */

/** @brief Stop the server
 *
 * This class provides a parser for the @c "exit" command, which asks the server
 * to terminate.
 */
class Exit : public Request::CommandParser {
public:
  /** @brief Constructor
   */
  Exit ();

  void run (std::ostream & cout);
};
/** @} */
}
}
