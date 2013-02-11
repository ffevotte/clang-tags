#include <clang-c/Index.h>
#include <string.h>
#include <stdlib.h>
#include <string>

CXChildVisitResult listSymbols (CXCursor cursor, CXCursor parent, CXClientData client_data) {
  std::string & targetFile = *((std::string*)client_data);

  // CXCursorKind kind = clang_getCursorKind (cursor);
  // CXString kindSpelling = clang_getCursorKindSpelling(kind);
  // CXString identifier = clang_getCursorSpelling(cursor);
  CXSourceLocation location = clang_getCursorLocation (cursor);
  CXFile file;
  unsigned int line, column, offset;
  clang_getExpansionLocation (location, &file, &line, &column, &offset);

  const char *filename = clang_getCString(clang_getFileName(file));
  if (not filename)
    return CXChildVisit_Continue;

  std::string fileStr (filename);
  if (fileStr != targetFile)
    return CXChildVisit_Continue;

  CXSourceRange extent = clang_getCursorExtent (cursor);
  CXSourceLocation end = clang_getRangeEnd (extent);
  unsigned int line2, column2;
  clang_getExpansionLocation (end, NULL, &line2, &column2, NULL);

  // fprintf (stderr, "%s:%d:%d-%d: %s %s\n",
  //          clang_getCString(clang_getFileName (file)),
  //          line, column, column2-1,
  //          clang_getCString(kindSpelling),
  //          clang_getCString(identifier));

  CXCursor cursorDef = clang_getCursorReferenced(cursor);
  const char * usr = clang_getCString(clang_getCursorUSR(cursorDef));
  if (strcmp(usr, ""))
    fprintf (stderr, "%s\t%s:%d-%d:%d-%d\n",
             usr,
             clang_getCString(clang_getFileName (file)),
             line, line2, column, column2-1);


  if (0) {
    CXCursor cursorDef = clang_getCursorReferenced (cursor);
    CXCursorKind kind = clang_getCursorKind (cursorDef);
    CXString kindSpelling = clang_getCursorKindSpelling(kind);
    CXString identifier = clang_getCursorSpelling(cursorDef);
    CXSourceLocation locationDef = clang_getCursorLocation (cursorDef);
    CXFile file;
    unsigned int line, column, offset;
    clang_getExpansionLocation (locationDef, &file, &line, &column, &offset);

    CXSourceRange extent = clang_getCursorExtent (cursorDef);
    CXSourceLocation end = clang_getRangeEnd (extent);
    unsigned int column2;
    clang_getExpansionLocation (end, NULL, NULL, &column2, NULL);

    const char * filename = clang_getCString(clang_getFileName (file));
    if (filename)
      fprintf (stderr, "%s:%d:%d-%d:\n\t%s %s\n\n",
               filename,
               line, column, column2-1,
               clang_getCString(kindSpelling),
               clang_getCString(identifier));
  }

  return CXChildVisit_Recurse;
}


CXChildVisitResult findDefinition (CXCursor cursor, CXCursor parent,
                                   CXClientData client_data) {
  CXSourceLocation * targetLocation = (CXSourceLocation*)client_data;
  CXSourceLocation location = clang_getCursorLocation (cursor);

  if (clang_equalLocations (location, *targetLocation)) {
    if (clang_isUnexposed(clang_getCursorKind(cursor)))
      return CXChildVisit_Recurse;
    // {
    //   CXCursorKind kind = clang_getCursorKind (cursor);
    //   CXString kindSpelling = clang_getCursorKindSpelling(kind);
    //   CXString identifier = clang_getCursorSpelling(cursor);
    //   CXSourceLocation location = clang_getCursorLocation (cursor);
    //   CXFile file;
    //   unsigned int line, column, offset;
    //   clang_getExpansionLocation (location, &file, &line, &column, &offset);

    //   CXSourceRange extent = clang_getCursorExtent (cursor);
    //   CXSourceLocation end = clang_getRangeEnd (extent);
    //   unsigned int column2;
    //   clang_getExpansionLocation (end, NULL, NULL, &column2, NULL);

    //   const char * filename = clang_getCString(clang_getFileName (file));
    //   if (filename)
    //     fprintf (stderr, "%s:%d:%d-%d: %s %s\n",
    //              filename,
    //              line, column, column2-1,
    //              clang_getCString(kindSpelling),
    //              clang_getCString(identifier));
    // }

    CXCursor cursorDef = clang_getCursorReferenced (cursor);
    CXCursorKind kind = clang_getCursorKind (cursorDef);
    CXString kindSpelling = clang_getCursorKindSpelling(kind);
    CXString identifier = clang_getCursorSpelling(cursorDef);
    CXSourceLocation locationDef = clang_getCursorLocation (cursorDef);
    CXFile file;
    unsigned int line, column, offset;
    clang_getExpansionLocation (locationDef, &file, &line, &column, &offset);

    CXSourceRange extent = clang_getCursorExtent (cursorDef);
    CXSourceLocation end = clang_getRangeEnd (extent);
    unsigned int line2, column2;
    clang_getExpansionLocation (end, NULL, &line2, &column2, NULL);

    const char * filename = clang_getCString(clang_getFileName (file));
    if (filename)
      fprintf (stderr, "%s:%d-%d:%d-%d: (%s)\n\t%s %s\n\n",
               filename,
               line, line2, column, column2-1,
               clang_getCString(clang_getCursorUSR(cursorDef)),
               clang_getCString(kindSpelling),
               clang_getCString(identifier));
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

  const int operation = 2;

  if (operation == 1) {
    std::string target (fileName);
    clang_visitChildren (cursor, listSymbols, &target);
  }

  if (operation == 2) {
    CXFile topFile = clang_getFile (TU, fileName);
    CXSourceLocation target = clang_getLocationForOffset (TU, topFile, offset);
    clang_visitChildren (cursor, findDefinition, &target);
  }

  clang_disposeTranslationUnit(TU);
  clang_disposeIndex(Index);
  return 0;
}
