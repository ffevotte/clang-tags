#include "inotifyMap.hxx"
namespace ClangTags {
namespace Watch {

void InotifyMap::add (const std::string & fileName, int wd) {
  wd_[fileName] = wd;
  file_[wd] = fileName;
}

std::string InotifyMap::fileName (int wd) {
  return file_[wd];
}

bool InotifyMap::contains (const std::string & fileName) {
  return wd_.count(fileName)>0;
}
}
}
