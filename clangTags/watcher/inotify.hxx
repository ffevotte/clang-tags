#pragma once

#include "config.h"
#if defined HAVE_INOTIFY

#include "watcher.hxx"

#include "MT/aFlag.hxx"

#include "clangTags/storage.hxx"
#include "clangTags/indexer/indexer.hxx"

namespace ClangTags {
namespace Watcher {

/** @brief Background thread watching source files
 */
class Inotify : public Watcher {
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
  Inotify (Indexer::Indexer & updateThread);
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
  Indexer::Indexer & indexer_;
};


// Definition of Inotify::Map
inline
void Inotify::Map::add (const std::string & fileName, int wd) {
  wd_[fileName] = wd;
  file_[wd] = fileName;
}

inline
std::string Inotify::Map::fileName (int wd) {
  return file_[wd];
}

inline
bool Inotify::Map::contains (const std::string & fileName) {
  return wd_.count(fileName)>0;
}
}
}

#endif //defined HAVE_INOTIFY
