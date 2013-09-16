#pragma once

#include "clangTags/storage.hxx"
#include "clangTags/cache.hxx"

namespace ClangTags {
namespace Indexer {
class Update {
public:
  Update (Storage & storage, Cache & cache);
  void operator() ();

private:
  Storage & storage_;
  Cache & cache_;
};
}
}
