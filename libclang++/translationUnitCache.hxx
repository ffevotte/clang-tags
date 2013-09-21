#pragma once

#include "translationUnit.hxx"
#include <list>
#include <map>

namespace LibClang {
  /** @addtogroup libclang
   * @{
   */

  /** @brief Provides caching of @c TranslationUnit instances.
   *
   * This class provides a memory-limited cache of translation units. When the
   * memory limit is exceeded, the least recently-used translation units are
   * disposed.
   */
  class TranslationUnitCache {
  public:
    /** @brief Constructor
     *
     * @param memoryLimit The maximum memory usage (in bytes) of the cache.
     */
    TranslationUnitCache (unsigned long memoryLimit);

    /** @brief Determine whether a cache entry exists.
     *
     * @param fileName full path to the source file
     * @return true if the cache contains a translation unit corresponding to
     *         the given filename.
     */
    bool contains (const std::string & fileName) const;

    /** @brief Add a new translation unit to the cache.
     *
     * Inserts the translation unit into the cache, and possibly disposes older translation
     * units in order to satisfy the memory usage limit.
     *
     * @param fileName full path to the source file
     * @param tu       translation unit associated to @c fileName
     */
    void insert (const std::string & fileName, const TranslationUnit & tu);

    /** @brief Retrieve a translation unit from the cache.
     *
     * Use @ref contains() to first determine whether the cache entry exists.
     *
     * @param fileName full path to the source file
     * @return the translation unit corresponding to @c filename in the cache
     */
    TranslationUnit & get (const std::string & fileName);

  private:
    const unsigned long memoryLimit_;
    unsigned long memoryUsage_;

    typedef std::list<std::string> LRUFileList;
    LRUFileList lruFiles_;
    std::map<std::string, std::pair<TranslationUnit, LRUFileList::iterator>> tunits_;
  };

  /** @} */
}
