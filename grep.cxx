#include "application.hxx"
#include "sourceFile.hxx"

void Application::grep (const GrepArgs & args, std::ostream & cout) {
  const auto refs = storage_.grep (args.usr);

  auto ref = refs.begin ();
  const auto end = refs.end ();
  for ( ; ref != end ; ++ref ) {
    SourceFile file (ref->file);
    cout << ref->file
         << ":" << ref->line1
         << ":" << file.line (ref->line1)
         << std::endl;
  }
}
