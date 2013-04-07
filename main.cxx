#include "application.hxx"
#include "request.hxx"

class CompilationDatabaseCommand : public Request::CommandParser {
public:
  CompilationDatabaseCommand (const std::string & name, Application & application)
    : Request::CommandParser (name, "Create a compilation database"),
      application_ (application)
  {
    prompt_ = "compilationDB> ";
  }

  void defaults () {
    args_.fileName = "compile_commands.json";
  }

  void run () {
    application_.compilationDatabase (args_);
  }

private:
  Application & application_;
  Application::CompilationDatabaseArgs args_;
};


class UpdateCommand : public Request::CommandParser {
public:
  UpdateCommand (const std::string & name, Application & application)
    : Request::CommandParser (name, "Update the source code index"),
      application_ (application)
  {
    prompt_ = "update> ";

    defaults();

    using Request::key;
    add (key ("diagnostics", args_.diagnostics)
         ->metavar ("true|false")
         ->description ("Print compilation diagnostics"));
  }

  void defaults () {
    args_.diagnostics = true;
  }

  void run () {
    application_.update (args_);
  }

protected:
  Application & application_;
  Application::IndexArgs args_;
};


class IndexCommand : public UpdateCommand {
public:
  IndexCommand (const std::string & name, Application & application)
    : UpdateCommand (name, application)
  {
    description_ = "Index the source code base";
    prompt_ = "index> ";

    defaults ();
    using Request::key;
    add (key ("exclude", args_.exclude)
         ->metavar ("PATH")
         ->description ("Exclude path"));
  }

  void defaults () {
    UpdateCommand::defaults();
    args_.exclude = {"/usr"};
  }

  void run () {
    application_.index (args_);
  }
};


class FindCommand : public Request::CommandParser {
public:
  FindCommand (const std::string & name, Application & application)
    : Request::CommandParser (name, "Find the definition of a symbol"),
      application_ (application)
  {
    prompt_ = "find> ";

    defaults ();

    using Request::key;
    add (key ("file", args_.fileName)
         ->metavar ("FILENAME")
         ->description ("Source file name"));
    add (key ("offset", args_.offset)
         ->metavar ("OFFSET")
         ->description ("Offset in bytes"));
  }

  void defaults () {
    args_.fileName = "";
    args_.offset = 0;
    args_.mostSpecific = false;
    args_.printDiagnostics = true;
  }

  void run () {
    application_.findDefinition (args_);
  }

private:
  Application & application_;
  Application::FindDefinitionArgs args_;
};


class CompleteCommand : public Request::CommandParser {
public:
  CompleteCommand (const std::string & name, Application & application)
    : Request::CommandParser (name, "Complete the code at point"),
      application_ (application)
  {
    prompt_ = "complete> ";

    defaults();

    using Request::key;
    add (key ("file", args_.fileName)
         ->metavar ("FILENAME"));
    add (key ("line", args_.line)
         ->metavar ("LINE_NO"));
    add (key ("column", args_.column)
         ->metavar ("COLUMN_NO"));
  }

  void defaults () {
    args_.fileName = "";
    args_.line = 0;
    args_.column = 0;
  }

  void run () {
    application_.complete (args_);
  }

private:
  Application & application_;
  Application::CompleteArgs args_;
};


int main () {
  Storage storage;

  Application app (storage);

  Request::Parser p ("DRUIDE\n"
                     "DRuide is an Un-Integrated Development Environment\n");
  p .add (new CompilationDatabaseCommand ("compilationDatabase", app))
    .add (new IndexCommand ("index", app))
    .add (new UpdateCommand ("update", app))
    .add (new FindCommand ("find", app))
    .add (new CompleteCommand ("complete", app))
    .prompt ("clang-dde> ")
    .parse (std::cin);

  return EXIT_SUCCESS;
}
