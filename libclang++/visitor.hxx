#pragma once

#include "cursor.hxx"

namespace LibClang {
  template <typename VISITOR>
  CXChildVisitResult visitChildren (CXCursor rawCursor,
                                    CXCursor rawParent, //unused
                                    CXClientData client_data)
  {
    const LibClang::Cursor cursor (rawCursor);
    const LibClang::Cursor parent (rawParent);
    VISITOR & visitor = *((VISITOR*)client_data);

    return visitor.visit (cursor, parent);
  }

  /** @addtogroup libclang
      @{
  */

  /** @brief Base class for AST visitors
   *
   * This class provides the needed interface for visiting an Abstract Syntax
   * Tree (AST). Every functor used to visit ASTs should derive from this using
   * a CRTP, and specialize the visit() method.
   *
   * Below is an example of a visitor functor printing the USR of all visited nodes:
   *
   * @code
   * // Define the visitor class
   * class MyVisitor : public LibClang::Visitor<MyVisitor> {
   * public:
   *   MyVisitor () { }
   *
   *   CXChildVisitResult visit (LibClang::Cursor cursor,
   *                             LibClang::Cursor parent)
   *   {
   *     std::cerr << cursor.USR();
   *     return CXChildVisit_Recurse;
   *   }
   * };
   *
   * // Use it to traverse a TranslationUnit
   * MyVisitor visitor;
   * TranslationUnit tu;
   * visitor.visitChildren (tu.cursor());
   * @endcode
   */
  template <typename VISITOR>
  class Visitor {
  public:
    /** @brief visit an AST
     *
     * Visit an Abastract Syntax Tree (AST), starting from a top Cursor and
     * recursively visiting its children. The visiting strategy and actions
     * taken for each Cursor in the AST are defined by the visit() method.
     *
     * @param top  a Cursor representing the AST to visit
     *
     * @return non-zero if the traversal was ended prematurely
     */
    unsigned int visitChildren (const Cursor & top)
    {
      return clang_visitChildren (top.raw(),
                                  LibClang::visitChildren<VISITOR>,
                                  (VISITOR*)this);
    }

    /** @brief visit a cursor in the AST and decide what to do next
     *
     * This method visits a Cursor in the AST, and returns a code indicating which action should be taken next:
     * - @c CXChildVisit_Recurse: continue recursively visiting the AST
     * - @c CXChildVisit_Continue: continue visiting the AST, but don't recurse into the current cursors children
     * - @c CXChildVisit_Break: stop traversing the AST
     *
     * @param cursor
     * @param parent
     *
     * @return a code indicating what to do next
     */
    virtual CXChildVisitResult visit (Cursor cursor, LibClang::Cursor parent) = 0;
  };

  /** @} */
}
