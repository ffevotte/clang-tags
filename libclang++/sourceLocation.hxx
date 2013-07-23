#pragma once
#include <clang-c/Index.h>
#include <string>

namespace LibClang {
  /** @addtogroup libclang
      @{
  */

  /** @brief Location in the source code
   *
   * This class is a proxy for libclang's @c CXSourceLocation type. It should
   * provide the same features.
   *
   * A physical source location is mainly identified by the parameters in a
   * Position structure, a retrieved by the expansionLocation() method.
   *
   * Note that a source location can be associated to different physical
   * locations, especially in the case of macro expansions, which can either
   * refer to the actual expansion, or the macro definition.
   */
  class SourceLocation {
  public:
    /** @brief Physical position in the source code
     *
     * A physical source location is identified by a file name, and a position
     * inside the file, which can be one of:
     * - an offset, counted in characters starting from the file beginning
     * - a (line, column) pair
     */
    struct Position {
      std::string file;         /**< @brief file name */
      unsigned int line;        /**< @brief line number */
      unsigned int column;      /**< @brief column number */
      unsigned int offset;      /**< @brief offset in characters since the file beginning */
    };

    /** @brief Equality operator
     *
     * @param other  SourceLocation to be compared with
     *
     * @return true if the two source locations are identical
     */
    bool operator== (const SourceLocation & other) const;

    /** @brief Get the associated physical position
     *
     * If the location refers to a macro expansion, return the position of the
     * expansion itself, instead of the macro definition.
     *
     * @return a Position structure
     */
    const Position expansionLocation () const;

  private:
    SourceLocation (CXSourceLocation raw);
    CXSourceLocation location_;
    const CXSourceLocation & raw () const;

    // Friend declaration
    friend class TranslationUnit;
    friend class Cursor;
  };

  /** @} */
}
