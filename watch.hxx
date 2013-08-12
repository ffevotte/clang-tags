#pragma once

#include "application.hxx"
#include <boost/thread/thread.hpp>

class Watch {
public:
  Watch (Application & application);
  ~Watch ();

  void update ();

  // Required for the callable concept
  void operator() ();

private:
  void addWatchDescriptor (const std::string & fileName, int wd);
  std::string fileName (int wd);

  Application & application_;
  int fd_inotify_;
  int fd_update_[2];
  std::map<std::string, int> wd_;
  std::map<int, std::string> file_;
  boost::mutex mtx_;
};
