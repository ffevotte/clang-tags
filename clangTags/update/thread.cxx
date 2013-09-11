#include "thread.hxx"
#include "MT/stream.hxx"

#include "clangTags/watch/thread.hxx"

namespace ClangTags {
namespace Update {

Thread::Thread (Cache & cache)
  : index_   (storage_, cache),
    indexRequested_ (true),
    indexUpdated_ (false),
    watchThread_ (NULL)
{}

void Thread::setWatchThread (Watch::Thread * watchThread)
{
  watchThread_ = watchThread;
}

void Thread::index () {
  indexRequested_.set (true);
}

void Thread::wait () {
  indexUpdated_.get();
}

void Thread::updateIndex_ () {
  // Reindex source files
  index_();

  // Notify the watching thread
  if (watchThread_)
    watchThread_->update();

  // Reset flag and notify waiting threads
  indexRequested_.set (false);
  indexUpdated_.set (true);
}

void Thread::operator() () {
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
