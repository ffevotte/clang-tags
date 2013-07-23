#pragma once

#include <clang-c/Index.h>
#include <memory>
#include <vector>
#include <string>
namespace LibClang {
  /** @addtogroup libclang
      @{
  */

  // Forward declaration
  class TranslationUnit;


  /** @brief Set of translation units
   *
   * This is a proxy for libclang's @c CXIndex type
   */
  class Index {
  public:
    /** @brief default constructor
     */
    Index ();

    /** @brief Create a translation unit from a command-line
     *
     * Return the translation unit for a given source file and the provided
     * command-line arguments which would be passed to the compiler.
     *
     * @param argc  The number of command-line arguments
     * @param argv  A C-style array of command-line arguments
     *
     * @return The corresponfing TranslationUnit object
     */
    TranslationUnit parse (int argc, char const *const *const argv) const;

    /** @brief Create a translation unit from a command-line
     *
     * Return the translation unit for a given source file and the provided
     * command-line arguments which would be passed to the compiler.
     *
     * @param args  A vector of command-line arguments
     *
     * @return The corresponfing TranslationUnit object
     */
    TranslationUnit parse (const std::vector<std::string> & args) const;

  private:
    const CXIndex & raw() const;
    struct Index_ {
      CXIndex index_;
      Index_ (CXIndex index) : index_ (index) {}
      ~Index_ () { clang_disposeIndex (index_); }
    };

    std::shared_ptr<Index_> index_;
  };

  /** @} */
}
