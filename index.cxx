#include "libclang++/libclang++.hxx"
#include "getopt++/getopt.hxx"

#include "util.hxx"
#include "application.hxx"

#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>

class Indexer : public LibClang::Visitor<Indexer> {
public:
  Indexer (const std::string & fileName,
           const std::vector<std::string> & exclude,
           Storage & storage)
    : sourceFile_ (fileName),
      exclude_    (exclude),
      storage_    (storage)
  {
    needsUpdate_[fileName] = storage.beginFile (fileName);
    storage_.addInclude (fileName, fileName);
  }

  CXChildVisitResult visit (LibClang::Cursor cursor,
                            LibClang::Cursor parent)
  {
    const LibClang::Cursor cursorDef (cursor.referenced());

    // Skip non-reference cursors
    if (cursorDef.isNull()) {
      return CXChildVisit_Recurse;
    }

    const std::string usr = cursorDef.USR();
    if (usr == "") {
      return CXChildVisit_Recurse;
    }

    const LibClang::SourceLocation::Position begin = cursor.location().expansionLocation();
    const String fileName = begin.file;

    if (fileName == "") {
      return CXChildVisit_Continue;
    }

    { // Skip excluded paths
      auto it  = exclude_.begin();
      auto end = exclude_.end();
      for ( ; it != end ; ++it) {
        if (fileName.startsWith (*it)) {
          return CXChildVisit_Continue;
        }
      }
    }

    if (needsUpdate_.count(fileName) == 0) {
      needsUpdate_[fileName] = storage_.beginFile (fileName);
      storage_.addInclude (fileName, sourceFile_);
    }

    if (needsUpdate_[fileName]) {
      const LibClang::SourceLocation::Position end = cursor.end().expansionLocation();
      storage_.addTag (usr, cursor.kindStr(), cursor.spelling(), fileName,
                       begin.line, begin.column, begin.offset,
                       end.line,   end.column,   end.offset,
                       cursor.isDeclaration());
    }

    return CXChildVisit_Recurse;
  }

private:
  const std::string              & sourceFile_;
  const std::vector<std::string> & exclude_;
  Storage                        & storage_;
  std::map<std::string, bool>      needsUpdate_;
};



void Application::index (IndexArgs & args, std::ostream & cout) {
  cout << std::endl
       << "-- Indexing project" << std::endl;
  storage_.setOption ("exclude", args.exclude);
  storage_.cleanIndex();

  updateIndex_ (args, cout);
}

void Application::update (IndexArgs & args, std::ostream & cout) {
  cout << std::endl
       << "-- Updating index" << std::endl;
  args.exclude = storage_.getOption ("exclude", Storage::Vector());

  updateIndex_ (args, cout);
}

void Application::updateIndex_ (IndexArgs & args, std::ostream & cout) {
  Timer totalTimer;

  storage_.beginIndex();
  std::string fileName;
  while ((fileName = storage_.nextFile()) != "") {
    cout << fileName << ":" << std::endl
         << "  parsing..." << std::flush;
    Timer timer;

    LibClang::TranslationUnit tu = translationUnit_(fileName);

    cout << "\t" << timer.get() << "s." << std::endl;
    timer.reset();

    // Print clang diagnostics if requested
    if (args.diagnostics) {
      for (unsigned int N = tu.numDiagnostics(),
             i = 0 ; i < N ; ++i) {
        cout << tu.diagnostic (i) << std::endl << std::endl;
      }
    }

    cout << "  indexing..." << std::flush;
    LibClang::Cursor top (tu);
    Indexer indexer (fileName, args.exclude, storage_);
    indexer.visitChildren (top);
    cout << "\t" << timer.get() << std::endl;
  }
  storage_.endIndex();
  cout << totalTimer.get() << "s." << std::endl;
}
