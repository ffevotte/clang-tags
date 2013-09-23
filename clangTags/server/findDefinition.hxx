#pragma once

#include "libclang++/libclang++.hxx"
#include "request/request.hxx"

#include "clangTags/storage.hxx"
#include "clangTags/cache.hxx"

#include <string>

namespace ClangTags {
namespace Server {
/** @addtogroup clangTags
 *  @{
 */

/** @brief Find the definition of a symbol in the sources
 *
 * This class provides a parser for the @c "find" command, which looks for an
 * identifier in a source file, and finds the definition it refers to.
 */
class FindDefinition : public Request::CommandParser {
public:
  /** @brief Constructor
   *
   * @param storage  @ref Storage instance to look for indexed tags
   * @param cache    @ref Cache instance storing translation units
   */
  FindDefinition (Storage & storage,
                  Cache & cache);

  void defaults ();

  /** @brief Find the definition of a symbol in the project sources
   *
   * Depending on the requests arguments, look for the symbol definition in the
   * @ref Storage database, or reparse the source file.
   *
   * @param cout  output stream used to display the results
   */
  void run (std::ostream & cout);

  /** @brief Pretty print an @ref Identifier
   *
   * This function prints all data related to an @ref Identifier in
   * human-readable format on the provided output stream.
   *
   * @param identifier @ref Identifier to print
   * @param cout       output stream
   */
  static void prettyPrint (const ClangTags::Identifier & identifier,
                           std::ostream & cout);

  /** @brief Display an @ref Identifier in machine-readable format
   *
   * This function prints all data related to an @ref Identifier in JSON format,
   * suitable for parsing by the client.
   *
   * @param identifier @ref Identifier to print
   * @param cout       output stream
   */
  static void output (const ClangTags::Identifier & identifier,
                      std::ostream & cout);

  /** @brief Display a Cursor in machine-readable format
   *
   * This function prints all data related to a @ref LibClang::Cursor "Cursor"
   * in JSON format, suitable for parsing by the client.
   *
   * @param cursor  @ref LibClang::Cursor instance to print
   * @param cout    output stream
   */
  static void output (LibClang::Cursor cursor,
                      std::ostream & cout);

private:
  void fromIndex_  (std::ostream & cout);
  void fromSource_ (std::ostream & cout);

  struct Args {
    std::string fileName;
    int         offset;
    bool        diagnostics;
    bool        mostSpecific;
    bool        fromIndex;
  };
  Args args_;

  Storage & storage_;
  Cache & cache_;
};
/** @} */
}
}
