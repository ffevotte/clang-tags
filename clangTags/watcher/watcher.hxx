#pragma once

#include "clangTags/indexer/indexer.hxx"

namespace ClangTags {
namespace Watcher {
/** @addtogroup clangTags
 *  @{
 */

/** @brief Interface for filesystem watchers
 *
 * This abstract class provides an interface for filesystem watching services.
 */
class Watcher {
public:

  /** @brief Constructor
   *
   * Watcher instances should be constructed with an initial index update
   * already scheduled (see update()).
   *
   * @param indexer  @ref Indexer::Indexer "Indexer" instance to notify
   */
  Watcher (Indexer::Indexer & indexer)
    : indexer_ (indexer)
  {}

  virtual ~Watcher () {};

  /** @brief Schedule an update of the watched source files
   *
   * Calling this method asks the Watcher instance to update its list of files
   * to watch on the filesystem. The source files list should be retrieved from
   * the compilation database stored in a Storage instance.
   */
  virtual void update () = 0;

protected:
  /** @brief Schedule a reindexing of the source code
   *
   * Derived classes should call this method to schedule a source code
   * reindexing upon detection of filesystem changes.
   */
  void reindex () {
    indexer_.index();
  }

private:
  Indexer::Indexer & indexer_;
};
/** @} */
}
}
