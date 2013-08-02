#include "sourceFile.hxx"

#include "libclang++/libclang++.hxx"

#include "application.hxx"

#include <iostream>
#include <cstdlib>

void displayRefDef (const Storage::RefDef & refDef, std::ostream & cout)
{
  const Storage::Reference  & ref = refDef.ref;
  const Storage::Definition & def = refDef.def;

  // Display reference
  {
    SourceFile sourceFile (ref.file);

    cout << "-- " << sourceFile.substring (ref.offset1, ref.offset2) << " -- "
         << ref.kind << " " << ref.spelling
         << std::endl;
  }

  // Display definition
  {
    cout << "   "
         << def.file  << ":"
         << def.line1 << "-" << def.line2 << ":"
         << def.col1  << "-" << def.col2-1 << ": "
         << def.kind  << " " << def.spelling
         << std::endl << "   USR: " << def.usr
         << std::endl << std::endl;
  }
}

void outputRefDef (const Storage::RefDef & refDef, std::ostream & cout)
{
  Json::FastWriter writer;
  Json::Value json = refDef.json();

  const Storage::Reference & ref = refDef.ref;
  SourceFile sourceFile (ref.file);
  json["ref"]["substring"] = sourceFile.substring (ref.offset1, ref.offset2);

  cout << writer.write (json);
}

void displayCursor (LibClang::Cursor cursor, std::ostream & cout)
{
  const LibClang::SourceLocation location (cursor.location());
  const LibClang::Cursor cursorDef = cursor.referenced();

  if (cursorDef.isNull()) {
    return;
  }

  Storage::RefDef refDef;
  Storage::Reference & ref = refDef.ref;
  Storage::Definition & def = refDef.def;

  // Get cursor information
  {
    const LibClang::SourceLocation::Position begin = location.expansionLocation();
    const LibClang::SourceLocation::Position end = cursor.end().expansionLocation();
    ref.file = begin.file;
    ref.offset1 = begin.offset;
    ref.offset2 = end.offset;
    ref.kind = cursor.kindStr();
    ref.spelling = cursor.spelling();
  }

  // Get referenced cursor information
  {
    const LibClang::SourceLocation::Position begin = cursorDef.location().expansionLocation();
    const LibClang::SourceLocation::Position end = cursorDef.end().expansionLocation();
    def.file = begin.file;
    def.line1 = begin.line;
    def.line2 = end.line;
    def.col1 = begin.column;
    def.col2 = end.column;
    def.kind = cursorDef.kindStr();
    def.spelling = cursorDef.spelling();
    def.usr = cursorDef.USR();
  }

  outputRefDef (refDef, cout);
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

void Application::findDefinitionFromIndex_ (FindDefinitionArgs & args, std::ostream & cout) {
  const auto refDefs = storage_.findDefinition (args.fileName, args.offset);
  auto refDef = refDefs.begin();
  const auto end = args.mostSpecific
    ? refDef + 1
    : refDefs.end();
  for ( ; refDef != end ; ++refDef ) {
    outputRefDef (*refDef, cout);
  }
}

void Application::findDefinitionFromSource_ (FindDefinitionArgs & args, std::ostream & cout) {
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

void Application::findDefinition (FindDefinitionArgs & args, std::ostream & cout) {
  if (args.fromIndex) {
    // Request references from the index database
    findDefinitionFromIndex_ (args, cout);
  }
  else {
    // Parse the source file and analyse it
    findDefinitionFromSource_ (args, cout);
  }
}
