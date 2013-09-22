#pragma once

#include "clangTags/storage.hxx"
#include "clangTags/cache.hxx"

namespace ClangTags {
namespace Indexer {
/** @addtogroup clangTags
 *  @{
 */

/** @brief Reindex source files
 */
class Update {
public:
  /** @brief Constructor
   *
   * @param storage  Storage instance
   * @param cache    Translation units Cache
   */
  Update (Storage & storage, Cache & cache);

  /** @brief Reindex the source files
   *
   * Retrieve from the compilation database all source and header files which
   * have been modified in the filesystem since last indexing. For each of them,
   * (re)parse and (re)index the associated translation unit.
   */
  void operator() ();

private:
  Storage & storage_;
  Cache & cache_;
};
/** @} */
}
}
