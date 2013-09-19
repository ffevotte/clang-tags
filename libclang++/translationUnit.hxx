#pragma once

#include <clang-c/Index.h>
#include <memory>

#include "unsavedFiles.hxx"

namespace LibClang {
  /** @addtogroup libclang
      @{
  */

  // Forward declarations
  class Index;
  class SourceLocation;
  class Cursor;

  /** @brief Translation unit
   *
   * This class is a proxy for libclang's \c CXTranslationUnit type and should
   * provide the same features, except its lifetime should not have to be
   * explicitly managed.
   *
   * The preferred way to create TranslationUnit objects is calling Index::parse.
   */
  class TranslationUnit {
  public:
    /** @brief Reparse the translation unit
     *
     * Re-parse the source files used to create the translation unit, reading
     * up-to-date source code from the file-system. The source code is re-parsed
     * with the same command-line options than the first time.
     */
    void reparse ();

    /** @brief Reparse the translation unit
     *
     * Re-parse the source files used to create the translation unit, reading
     * up-to-date content from the set of in-memory buffers in @em unsaved. The
     * source code is re-parsed with the same command-line options than the
     * first time.
     *
     * @param unsaved  A set of unsaved contents for the source files
     */
    void reparse (UnsavedFiles & unsaved);

    /** @brief Get the source location for a file/offset in the translation unit
     *
     * Retrieve the source location associated to a given source file and
     * character offset in the translation unit.
     *
     * @param fileName  The source file name
     * @param offset    The offset, counted in characters from the file beginning
     *
     * @return A SourceLocation object for the relevant location.
     */
    SourceLocation getLocation (const char* fileName, unsigned int offset);

    /** @brief Get a cursor to the translation unit
     *
     * This cursor can be used to start traversing the Abstract Syntax Tree
     * (AST) of the translation unit.
     *
     * @return A Cursor representing the whole translation unit
     */
    Cursor cursor () const;

    /** @brief Get the number of diagnostic messages for the translation unit
     *
     * These diagnostic messages are those emitted by the compiler when parsing
     * the translation unit.
     *
     * @return The number of diagnostic messages
     */
    unsigned int numDiagnostics ();

    /** @brief Get the i-th diagnostic message for the translation unit
     *
     * These diagnostic messages are those emitted by the compiler when parsing
     * the translation unit.
     *
     * @param i  index of the diagnostic message (must be less than numDiagnostics())
     *
     * @return A string representing the message
     */
    std::string diagnostic (unsigned int i);

    /** @brief Get the memory usage of the translation unit.
     *
     * @return The memory usage (in bytes) of the translation unit.
     */
    unsigned long memoryUsage () const;

    // TODO Make this method private
    const CXTranslationUnit & raw () const;

  private:
    TranslationUnit (CXTranslationUnit tu);

    struct TranslationUnit_ {
      CXTranslationUnit translationUnit_;
      TranslationUnit_ (CXTranslationUnit tu) : translationUnit_ (tu) {}
      ~TranslationUnit_ () { clang_disposeTranslationUnit (translationUnit_); }
    };
    std::shared_ptr<TranslationUnit_> translationUnit_;

    // Friend declaration
    friend class Index;
    friend class Cursor;
  };

  /** @} */
}
