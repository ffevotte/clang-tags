#pragma once
#include <clang-c/Index.h>
#include <string>

namespace LibClang {
  /** @addtogroup libclang
      @{
  */

  // Forward declarations
  class TranslationUnit;
  class SourceLocation;

  /** @brief Node representing an entity in the AST
   *
   * This class is a proxy for libclang's @c CXCursor type. It should provide
   * the same features.
   *
   * A cursor represents an entity in the Abstract Syntax Tree (AST) of a
   * translation unit: statement, declaration, expressions, ... A cursor is
   * associated to a physical location in the source code, and to an Unified
   * Symbol Resolution (USR), which uniquely identifies the symbol it references
   * across translation units.
   */
  class Cursor {
  public:
    /** @brief Constructor from a translation unit
     *
     * Create a cursor representing the whole translation unit. This cursor can
     * be used to start visiting the translation unit.
     *
     * @param tu  the TranslationUnit
     */
    Cursor (const TranslationUnit & tu);

    /** @brief Constructor from a source location
     *
     * Create a cursor to the entity located at a specified location in the
     * source code of a translation unit. The returned cursor represents the
     * most specific entity spanning this location (i.e. a cursor which spans
     * the given location and has no child nodes in the AST).
     *
     * Creates a null cursor if no such location is found.
     *
     * @param tu        the TranslationUnit of the source code location
     * @param fileName  the file name of the source file in the translation unit
     * @param offset    the position in the file, expressed in characters since the file beginning
     */
    Cursor (const TranslationUnit & tu,
            const std::string & fileName, const unsigned int offset);

    /** @brief Determine whether the cursor is null
     *
     * A null cursor is used by some functions as a return value for invalid
     * cursors.
     *
     * @return true if the cursor is null
     */
    bool isNull () const;

    /** @brief Determine whether the cursor is unexposed
     *
     * Some entities in the AST do not (yet) have a specified kind; these are
     * the unexposed entities.
     *
     * @return Determine whether the cursor is unexposed
     */
    bool isUnexposed () const;

    /** @brief Determine whether the cursor represents a declaration
     *
     * @return true if the cursor represents a declaration
     */
    bool isDeclaration () const;

    /** @brief Get the cursor referenced
     *
     * For cursors which represent references to other entities in the AST,
     * retrieve the referenced cursor. For example, a cursor pointing to the use
     * of a variable refers to the declaration of this variable.
     *
     * @return the referenced Cursor, null if no such cursor can be found
     */
    Cursor referenced () const;

    /** @brief Get the kind of cursor
     *
     * Retrieve the kind of entity represented by the cursor, as a
     * string. Cursor kinds include for example:
     * - "ClassDecl": a class declaration
     * - "CXXMethod": a C++ class method
     * - "MemberRefExpr": an expression referring to a member of a class
     *
     * @return the cursor kind, as a string
     */
    std::string kindStr () const;

    /** @brief Get the name of the entity referred to
     *
     * @return the name of the referenced entity, as a string
     */
    std::string spelling () const;

    /** @brief Get the Unified Symbol Resolution for the entity referenced
     *
     * A Unified Symbol Resolution (USR) is a string that identifies a
     * particular entity (function, class, variable, etc.) within a
     * program. USRs can be compared across translation units to determine,
     * e.g., when references in one translation refer to an entity defined in
     * another translation unit.
     *
     * @return the USR, as a string
     */
    std::string USR () const;

    /** @brief Get the source location associated to a cursor
     *
     * The location returned corresponds to the first character of the
     * referenced entity in the source code. Use end() to get the location of
     * the last character, and thus the span of the entity in the source code.
     *
     * @return a SourceLocation to the beginning of the referenced entity
     */
    SourceLocation location () const;

    /** @brief Get the source location associated to a cursor
     *
     * The location returned corresponds to the last character of the referenced
     * entity in the source code. Use location() to get the location of the
     * first character, and thus the span of the entity in the source code.
     *
     * @return a SourceLocation to the end of the referenced entity
     */
    SourceLocation end () const;

  private:
    Cursor (CXCursor raw);
    const CXCursor & raw () const;
    CXCursor cursor_;

    // Friend declaration
    template <typename VISITOR>
    friend CXChildVisitResult visitChildren (CXCursor rawCursor,
                                             CXCursor rawParent, //unused
                                             CXClientData client_data);
    template <typename VISITOR>
    friend class Visitor;
  };

  /** @} */
}
