#pragma once

#include "libclang++/libclang++.hxx"
#include "libclang++/translationUnitCache.hxx"
#include "storage.hxx"
#include <unistd.h>

namespace ClangTags {
/** @addtogroup clangTags
 *  @{
 */

/** @brief Translation units cache
 *
 * @sa LibClang::TranslationUnitCache
 */
class Cache {
public:
  /** @brief Constructor
   *
   * @param cacheLimit maximum cache size (in bytes)
   */
  Cache (unsigned int cacheLimit)
    : tu_ (cacheLimit)
  {}

  /** @brief Get the translation unit associated to a source file
   *
   * If the translation unit is in cache, it is reparsed. Otherwise, the source
   * file is parsed from scratch and the translation unit is added to the cache.
   *
   * @param storage  Storage from which compilation commands can be retrieved
   * @param fileName full path to the source file
   *
   * @return an up-to-date LibClang::TranslationUnit associated to @c fileName
   */
  LibClang::TranslationUnit & translationUnit (Storage & storage,
                                               std::string fileName) {
    std::string directory;
    std::vector<std::string> clArgs;
    storage.getCompileCommand (fileName, directory, clArgs);

    // chdir() to the correct directory
    // (whether we need to parse the TU for the first time or reparse it)
    chdir (directory.c_str());

    if (!tu_.contains (fileName)) {
      LibClang::TranslationUnit tu = index_.parse (clArgs);
      tu_.insert (fileName, tu);
      return tu_.get (fileName);
    } else {
      LibClang::TranslationUnit & tu = tu_.get (fileName);
      tu.reparse();
      return tu;
    }
  }

private:
  LibClang::Index index_;
  LibClang::TranslationUnitCache tu_;
};

/** @} */
}
