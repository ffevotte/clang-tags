#pragma once

#include "clangTags/storage.hxx"
#include "clangTags/watcher/watcher.hxx"
#include "update.hxx"
#include "MT/sFlag.hxx"

namespace ClangTags {
namespace Indexer {

/** @brief Background thread updating the index
 */
class Indexer {
public:

  /** @brief Constructor
   *
   * This object accesses the database using its own @ref Storage::SqliteDB handle.
   *
   * @ref Update objects are constructed with an initial index update already
   * scheduled (see index()).
   *
   * @param cache @ref LibClang::TranslationUnit "TranslationUnit" cache
   */
  Indexer (Cache & cache);

  /** @brief Main loop
   *
   * This is the main loop executed by the thread. It watches for:
   * - index update requests coming from other threads (see index())
   * - notifications that sources have changed on the file system (using Linux's
   *   @c inotify service)
   */
  void operator() ();

  /** @brief Schedule an index update
   *
   * Calling this method asks the Update thread to reindex source files and
   * update its watch list.
   */
  void index ();

  /** @brief Wait until the index is updated
   *
   * Calling this method puts the current thread to sleep until the Update
   * thread is done reindexing source files.
   */
  void wait ();

  void setWatcher (Watcher::Watcher * watcher);

private:
  void updateIndex_();

  Storage storage_;
  Update update_;
  MT::SFlag<bool> indexRequested_;
  MT::SFlag<bool> indexUpdated_;
  Watcher::Watcher * watcher_;
};
}
}
