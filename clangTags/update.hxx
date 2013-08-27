#pragma once

#include "clangTags/storage/sqliteDB.hxx"
#include "clangTags/index.hxx"
#include <boost/lockfree/queue.hpp>

namespace ClangTags {

/** @brief Background thread updating the index
 */
class Update {
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
  Update (Cache & cache);
  ~Update ();

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

private:
  void updateWatchList_ ();

  class InotifyMap {
  public:
    void add (const std::string & fileName, int wd);
    std::string fileName (int wd);
    bool contains (const std::string & fileName);

  private:
    std::map<std::string, int> wd_;
    std::map<int, std::string> file_;
  };

  Storage::SqliteDB storage_;
  Index index_;
  int fd_inotify_;
  InotifyMap inotifyMap_;
  boost::lockfree::queue<int> queue_;
};
}
