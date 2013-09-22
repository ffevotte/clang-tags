#pragma once

#include "clangTags/storage.hxx"
#include "update.hxx"
#include "MT/sFlag.hxx"

namespace ClangTags { namespace Watcher { class Watcher; }}

namespace ClangTags {
namespace Indexer {
/** @addtogroup clangTags
 *  @{
 */

/** @brief Updates the source code index
 *
 * This class implements the @c callable concept, and is destined to be invoked
 * in a separate thread of execution.
 */
class Indexer {
public:

  /** @brief Constructor
   *
   * This object accesses the database using its own @ref Storage instance.
   *
   * @ref Indexer objects are constructed with an initial index update already
   * scheduled (see index()).
   *
   * @param cache @ref LibClang::TranslationUnit "TranslationUnit" cache
   */
  Indexer (Cache & cache);

  /** @brief Main loop
   *
   * This is the main loop executed by the thread. It watches for requests
   * coming from other threads (see index()), and updates the source files
   * index (see Update)
   */
  void operator() ();

  /** @brief Schedule an index update
   *
   * Calling this method asks the Indexer to update the source files index.
   */
  void index ();

  /** @brief Wait until the index is updated
   *
   * Upon calling this method, the calling thread is put to sleep until the
   * source files index has been updated.
   */
  void wait ();

  /** @brief Register a watcher instance
   *
   * After the index has been updated, if a @ref Watcher::Watcher "Watcher"
   * instance has been registered, it is notified to update its watch list.
   *
   * @param watcher  pointer to a @ref Watcher::Watcher "Watcher" instance
   */
  void setWatcher (Watcher::Watcher * watcher);

private:
  void updateIndex_();

  Storage storage_;
  Update update_;
  MT::SFlag<bool> indexRequested_;
  MT::SFlag<bool> indexUpdated_;
  Watcher::Watcher * watcher_;
};
/** @} */
}
}
