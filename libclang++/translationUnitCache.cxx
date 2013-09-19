#include "translationUnitCache.hxx"

namespace LibClang {
  TranslationUnitCache::TranslationUnitCache (unsigned long memoryLimit)
    : memoryLimit_(memoryLimit),
      memoryUsage_(0)
  {
  }

  bool TranslationUnitCache::contains (const std::string & fileName) const {
    return tunits_.find(fileName) != tunits_.end();
  }

  void TranslationUnitCache::insert (const std::string & fileName,
      const TranslationUnit & tu) {

    memoryUsage_ += tu.memoryUsage();

    // Clear out recently-used files until we have enough space for the new
    // translation unit.
    while (memoryUsage_ > memoryLimit_ && !lruFiles_.empty()) {
      auto it = tunits_.find(lruFiles_.front());
      memoryUsage_ -= it->second.first.memoryUsage();
      tunits_.erase(it);
      lruFiles_.pop_front();
    }

    // Add in our new translation unit. Even if the memory usage of this single
    // translation unit exceeds the memory limit, we will always insert it.
    auto it = lruFiles_.emplace(lruFiles_.end(), fileName);
    tunits_.emplace(fileName, std::make_pair(tu, it));
  }

  TranslationUnit & TranslationUnitCache::get (const std::string & fileName) {
    auto & entry = tunits_.find(fileName)->second;

    // Move this file to the end of the least-recently-used list.
    lruFiles_.splice(lruFiles_.end(), lruFiles_, entry.second);

    return entry.first;
  }
}
