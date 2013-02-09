#include <clang-c/Index.h>
#include <string.h>
#include <stdlib.h>

enum CXChildVisitResult visitor (CXCursor cursor, CXCursor parent, CXClientData client_data) {
  CXSourceLocation * targetLocation = client_data;
  CXSourceLocation location = clang_getCursorLocation (cursor);

  if (clang_equalLocations (location, *targetLocation)) {
    CXCursor cursorDef = clang_getCursorReferenced (cursor);

    enum CXCursorKind kind = clang_getCursorKind (cursorDef);
    CXString kindSpelling = clang_getCursorKindSpelling(kind);
    CXString identifier = clang_getCursorSpelling(cursorDef);
    CXSourceLocation locationDef = clang_getCursorLocation (cursorDef);
    CXFile file;
    unsigned int line, column, offset;
    clang_getExpansionLocation (locationDef, &file, &line, &column, &offset);

    fprintf (stderr, "%s:%d:%d: (offset=%d) %s %s\n",
             clang_getCString(clang_getFileName (file)),
             line, column, offset,
             clang_getCString(kindSpelling),
             clang_getCString(identifier));

    return CXChildVisit_Break;
  }

  return CXChildVisit_Recurse;
}

int main(int argc, char *argv[]) {
  char * fileName = argv[1];
  int offset = atol (argv[2]);

  CXIndex Index = clang_createIndex(0, 0);
  CXTranslationUnit TU = clang_parseTranslationUnit(Index, 0,
                                                    argv+2, argc-2,
                                                    0, 0, CXTranslationUnit_None);

  CXCursor cursor = clang_getTranslationUnitCursor (TU);
  CXFile topFile = clang_getFile (TU, fileName);
  CXSourceLocation target = clang_getLocationForOffset (TU, topFile, offset);
  clang_visitChildren (cursor, visitor, &target);

  clang_disposeTranslationUnit(TU);
  clang_disposeIndex(Index);
  return 0;
}
