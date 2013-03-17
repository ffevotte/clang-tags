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

  template <typename VISITOR>
  class Visitor {
  public:
    unsigned int visitChildren (const Cursor & top)
    {
      return clang_visitChildren (top.raw(),
                                  LibClang::visitChildren<VISITOR>,
                                  (VISITOR*)this);
    }
  };
}
