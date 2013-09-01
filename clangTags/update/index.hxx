#pragma once

#include "clangTags/storage.hxx"
#include "clangTags/cache.hxx"

namespace ClangTags {
namespace Update {
class Index {
public:
  Index (Storage::Interface & storage, Cache & cache);
  void operator() ();

private:
  Storage::Interface & storage_;
  Cache & cache_;
};
}
}
