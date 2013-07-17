#include "sourceFile.hxx"

#include "libclang++/libclang++.hxx"

#include "application.hxx"

#include <iostream>
#include <cstdlib>

void displayCursor (LibClang::Cursor cursor, std::ostream & cout)
{
  const LibClang::SourceLocation location (cursor.location());
  const LibClang::Cursor cursorDef = cursor.referenced();

  if (cursorDef.isNull()) {
    return;
  }

  // Display cursor
  {
    const LibClang::SourceLocation::Position begin = location.expansionLocation();
    const LibClang::SourceLocation::Position end = cursor.end().expansionLocation();
    SourceFile sourceFile (begin.file);

    cout << "-- " << sourceFile.substring (begin.offset, end.offset) << " -- "
         << cursor.kindStr() << " " << cursor.spelling()
         << std::endl;
  }

  // Display referenced cursor
  {
    const LibClang::SourceLocation::Position begin = cursorDef.location().expansionLocation();
    const LibClang::SourceLocation::Position end = cursorDef.end().expansionLocation();

    cout << "   "
         << begin.file << ":"
         << begin.line << "-" << end.line << ":"
         << begin.column << "-" << end.column-1 << ": "
         << cursorDef.kindStr() << " " << cursorDef.spelling()
         << std::endl << "   USR: " << cursorDef.USR()
         << std::endl << std::endl;
  }
}

class FindDefinition : public LibClang::Visitor<FindDefinition>
{
public:
  FindDefinition (const LibClang::SourceLocation & targetLocation,
                  std::ostream & cout)
    : targetLocation_ (targetLocation),
      cout_ (cout)
  {}

  CXChildVisitResult visit (LibClang::Cursor cursor,
                            LibClang::Cursor parent)
  {
    const LibClang::SourceLocation location (cursor.location());

    // Skip unexposed cursor kinds
    if (cursor.isUnexposed()) {
      return CXChildVisit_Recurse;
    }

    if (location == targetLocation_) {
      displayCursor (cursor, cout_);
    }

    return CXChildVisit_Recurse;
  }

private:
  const LibClang::SourceLocation & targetLocation_;
  std::ostream & cout_;
};

void Application::findDefinition (FindDefinitionArgs & args, std::ostream & cout) {
  std::string directory;
  std::vector<std::string> clArgs;
  storage_.getCompileCommand (args.fileName, directory, clArgs);

  LibClang::Index index;
  LibClang::TranslationUnit tu = index.parse (clArgs);

  // Print clang diagnostics if requested
  if (args.diagnostics) {
    for (unsigned int N = tu.numDiagnostics(),
           i = 0 ; i < N ; ++i) {
      cout << tu.diagnostic (i) << std::endl << std::endl;
    }
  }

  // Print cursor definition
  LibClang::Cursor cursor (tu, args.fileName.c_str(), args.offset);
  if (args.mostSpecific) {
    displayCursor (cursor, cout);
  }
  else {
    LibClang::SourceLocation target = cursor.location();
    FindDefinition findDef (target, cout);
    findDef.visitChildren (tu.cursor());
  }
}
