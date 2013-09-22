#pragma once

#include "config.h"
#if defined HAVE_INOTIFY

#include "watcher.hxx"

#include "MT/aFlag.hxx"

#include "clangTags/storage.hxx"

namespace ClangTags {
namespace Watcher {
/** @addtogroup clangTags
 *  @{
 */

/** @brief Implementation of the Watcher interface using Linux' inotify service.
 *
 * An Inotify instance is destined to be invoked as functor in a separate thread
 * of execution.
 */
class Inotify : public Watcher {
public:

  /** @brief Constructor
   *
   * This object accesses the database using its own @ref Storage handle.
   *
   * Inotify instances are constructed with an initial index update already
   * scheduled (see update()).
   *
   * @param indexer  @ref Indexer::Indexer "Indexer" instance to notify
   */
  Inotify (Indexer::Indexer & indexer);

  ~Inotify ();

  /** @brief Main loop
   *
   * This is the main loop executed by the thread. It watches for:
   * - source files list update requests coming from other threads (see update())
   * - notifications that sources have changed on the file system (using Linux's
   *   @c inotify service)
   */
  void operator() ();

  /** @brief Schedule an update of the watched source files
   *
   * Calling this method asks the Watch thread to update its list of source
   * files to watch
   */
  void update ();

private:
  void update_();

  class Map {
  public:
    void add (const std::string & fileName, int wd);
    std::string fileName (int wd);
    bool contains (const std::string & fileName);

  private:
    std::map<std::string, int> wd_;
    std::map<int, std::string> file_;
  };

  Storage storage_;
  int fd_inotify_;
  Map inotifyMap_;
  MT::AFlag<bool> updateRequested_;
};
/** @} */
}
}

#endif //defined HAVE_INOTIFY
