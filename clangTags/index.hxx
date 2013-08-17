#pragma once

#include "storage.hxx"
#include "cache.hxx"

namespace ClangTags {
class Index {
public:
  Index (Storage & storage, Cache & cache);
  void operator() (std::ostream & cout);

private:
  Storage & storage_;
  Cache   & cache_;
};
}
