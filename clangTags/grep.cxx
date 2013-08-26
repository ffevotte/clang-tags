#include "grep.hxx"
#include "sourceFile.hxx"

namespace ClangTags {

Grep::Grep (Storage::Interface & storage)
  : storage_ (storage)
{}

void Grep::operator() (const Args & args, std::ostream & cout) {
  Json::FastWriter writer;

  const auto refs = storage_.grep (args.usr);
  auto ref = refs.begin ();
  const auto end = refs.end ();
  for ( ; ref != end ; ++ref ) {
    Json::Value json = ref->json();
    SourceFile file (ref->file);
    json["lineContents"] = file.line (ref->line1);
    cout << writer.write (json);
  }
}
}
