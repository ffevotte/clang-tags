#include "watch.hxx"

#include <sys/inotify.h>
#include <sys/poll.h>

namespace ClangTags {
Watch::Watch (Storage & storage, Cache & cache)
  : storage_ (storage),
    index_   (storage, cache)
{
  // Initialize inotify
  fd_inotify_ = inotify_init ();
  if (fd_inotify_ == -1) {
    perror ("inotify_init");
    throw std::runtime_error ("inotify");
  }
}

Watch::~Watch () {
  // Close inotify file
  // -> the kernel will clean up all associated watches
  ::close (fd_inotify_);
}

void Watch::addWatchDescriptor (const std::string & fileName, int wd) {
  boost::lock_guard<boost::mutex> guard (mtx_);

  wd_[fileName] = wd;
  file_[wd] = fileName;
}

std::string Watch::fileName (int wd) {
  boost::lock_guard<boost::mutex> guard (mtx_);

  return file_[wd];
}


void Watch::update () {
  std::cerr << "Updating watchlist..." << std::endl;
  std::vector<std::string> list = storage_.listFiles();
  for (auto it=list.begin() ; it!=list.end() ; ++it) {
    std::string fileName = *it;

    // Skip already watched files
    if (wd_.count(fileName) > 0) {
      continue;
    }

    std::cerr << "Watching " << fileName << std::endl;
    int wd = inotify_add_watch (fd_inotify_, fileName.c_str(), IN_MODIFY);
    if (wd == -1) {
      perror ("inotify_add_watch");
    }

    addWatchDescriptor (fileName, wd);
  }
}

void Watch::operator() () {
  update();

  const size_t BUF_LEN = 1024;
  char buf[BUF_LEN] __attribute__((aligned(4)));

  for ( ; ; ) {
    ssize_t len, i = 0;
    len = read (fd_inotify_, buf, BUF_LEN);
    if (len < 1) {
      perror ("read");
      return;
    }

    while (i<len) {
      // Read event & update index
      struct inotify_event *event = (struct inotify_event *) &(buf[i]);
      i += sizeof (struct inotify_event) + event->len;

      std::cerr << "Detected modification of " << fileName(event->wd) << std::endl;
    }

    index_ (std::cerr);
  }
}
}
