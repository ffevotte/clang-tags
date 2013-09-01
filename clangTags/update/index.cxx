#include "libclang++/libclang++.hxx"
#include "getopt++/getopt.hxx"

#include "util/util.hxx"
#include "index.hxx"
#include "MT/stream.hxx"

#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>

namespace ClangTags {
namespace Update {

class Indexer : public LibClang::Visitor<Indexer> {
public:
  Indexer (const std::string & fileName,
           const std::vector<std::string> & exclude,
           Storage::Interface & storage)
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
      const bool needsUpdate = storage_.beginFile (fileName);
      needsUpdate_[fileName] = needsUpdate;
      if (needsUpdate) {
        MT::cerr() << "    " << fileName << std::endl;
      }
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
  Storage::Interface             & storage_;
  std::map<std::string, bool>      needsUpdate_;
};


Index::Index (Storage::Interface & storage, Cache & cache)
  : storage_ (storage),
    cache_   (cache)
{}

void Index::operator() () {
  Timer totalTimer;
  double parseTime = 0;
  double indexTime = 0;

  MT::cerr() << std::endl
             << "-- Updating index" << std::endl;

  std::vector<std::string> exclude;
  storage_.getOption ("index.exclude", exclude);

  bool diagnostics;
  storage_.getOption ("index.diagnostics", diagnostics);

  std::string fileName;
  while ((fileName = storage_.nextFile()) != "") {
    MT::cerr() << fileName << ":" << std::endl
               << "  parsing..." << std::flush;
    Timer timer;

    LibClang::TranslationUnit tu = cache_.translationUnit (storage_, fileName);

    double elapsed = timer.get();
    MT::cerr() << "\t" << elapsed << "s." << std::endl;
    parseTime += elapsed;

    // Print clang diagnostics if requested
    if (diagnostics) {
      for (unsigned int N = tu.numDiagnostics(),
             i = 0 ; i < N ; ++i) {
        MT::cerr() << tu.diagnostic (i) << std::endl << std::endl;
      }
    }

    MT::cerr() << "  indexing..." << std::endl;
    timer.reset();
    storage_.beginIndex();
    LibClang::Cursor top (tu);
    Indexer indexer (fileName, exclude, storage_);
    indexer.visitChildren (top);
    storage_.endIndex();

    elapsed = timer.get();
    MT::cerr() << "  indexing...\t" << elapsed << "s." << std::endl;
    indexTime += elapsed;
  }

  MT::cerr() << "Parsing time:  " << parseTime << "s." << std::endl
             << "Indexing time: " << indexTime << "s." << std::endl
             << "TOTAL:         " << totalTimer.get() << "s." << std::endl;
}
}
}
