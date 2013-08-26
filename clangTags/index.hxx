#pragma once

#include "storage.hxx"
#include "cache.hxx"

namespace ClangTags {
class Index {
public:
  Index (Storage::Interface & storage, Cache & cache);
  void operator() (std::ostream & cout);

private:
  Storage::Interface & storage_;
  Cache & cache_;
};
}
