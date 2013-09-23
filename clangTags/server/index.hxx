#pragma once

#include "request/request.hxx"

#include "clangTags/indexer/indexer.hxx"

namespace ClangTags {
namespace Server {
/** @addtogroup clangTags
 *  @{
 */

/** @brief Request an index update and wait for its completion
 *
 * This class provides a parser for the @c "index" command, which asks the @ref
 * Indexer to update the source files index, and wait for the reindexing to
 * terminate.
 */
class Index : public Request::CommandParser {
public:
  /** @brief Constructor
   *
   * @param indexer  @ref Indexer instance to use
   */
  Index (Indexer::Indexer & indexer);

  void run (std::ostream & cout);

private:
  Indexer::Indexer & indexer_;
};
/** @} */
}
}
