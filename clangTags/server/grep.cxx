#include "grep.hxx"
#include "sourceFile.hxx"

namespace ClangTags {
namespace Server {

Grep::Grep (Storage & storage)
  : Request::CommandParser ("grep", "Find all references to a definition"),
    storage_ (storage)
{
  prompt_ = "grep> ";
  defaults();

  using Request::key;
  add (key ("usr", args_.usr)
       ->metavar ("USR")
       ->description ("Unified Symbol Resolution for the symbol"));
}

void Grep::defaults () {
  args_.usr = "c:@F@main";
}

void Grep::run (std::ostream & cout) {
  Json::FastWriter writer;

  const auto refs = storage_.grep (args_.usr);
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
}
