#pragma once

#include "index.hxx"
#include "translationUnit.hxx"
#include "cursor.hxx"
#include "sourceLocation.hxx"
#include "visitor.hxx"

/** @addtogroup libclang LibClang++
    @brief C++ wrapper around libclang's C API.

@code
// Define the visitor class
class MyVisitor : public LibClang::Visitor<MyVisitor> {
public:
  MyVisitor () { }

  CXChildVisitResult visit (LibClang::Cursor cursor,
                            LibClang::Cursor parent)
  {
    std::cerr << cursor.USR();
    return CXChildVisit_Recurse;
  }
};

// Use it to traverse a TranslationUnit
MyVisitor visitor;
TranslationUnit tu;
visitor.visitChildren (tu.cursor());
@endcode

*/
