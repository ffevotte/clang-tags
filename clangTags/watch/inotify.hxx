#pragma once

#include "thread.hxx"
#include "inotifyMap.hxx"

#include "MT/aFlag.hxx"

#include "clangTags/storage/sqliteDB.hxx"
#include "clangTags/update/thread.hxx"

namespace ClangTags {
namespace Watch {

/** @brief Background thread watching source files
 */
class Inotify : public Thread {
public:

  /** @brief Constructor
   *
   * This object accesses the database using its own @ref Storage::SqliteDB handle.
   *
   * Objects are constructed with an initial index update already scheduled (see
   * update()).
   *
   * @param cache @ref LibClang::TranslationUnit "TranslationUnit" cache
   */
  Inotify (Update::Thread & updateThread);
  ~Inotify ();

  /** @brief Main loop
   *
   * This is the main loop executed by the thread. It watches for:
   * - source files list update requests coming from other threads (see index())
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

  Storage::SqliteDB storage_;
  int fd_inotify_;
  InotifyMap inotifyMap_;
  MT::AFlag<bool> updateRequested_;
  Update::Thread & updateThread_;
};
}
}
