#include "indexer.hxx"

#include "clangTags/watcher/watcher.hxx"

namespace ClangTags {
namespace Indexer {

Indexer::Indexer (Cache & cache)
  : update_         (storage_, cache),
    indexRequested_ (true),
    indexUpdated_   (false),
    watcher_        (NULL)
{}

void Indexer::setWatcher (Watcher::Watcher * watcher)
{
  watcher_ = watcher;
}

void Indexer::index () {
  indexRequested_.set (true);
}

void Indexer::wait () {
  indexUpdated_.get();
}

void Indexer::updateIndex_ () {
  // Reindex source files
  update_();

  // Notify the watching thread
  if (watcher_)
    watcher_->update();

  // Reset flag and notify waiting threads
  indexRequested_.set (false);
  indexUpdated_.set (true);
}

void Indexer::operator() () {
  updateIndex_();

  for ( ; ; ) {
    // Check whether a re-indexing was requested
    if (indexRequested_.get() == true) {
      updateIndex_();
    }
  }
}
}
}
