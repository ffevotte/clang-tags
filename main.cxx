#include "application.hxx"
#include "request.hxx"
#include <asio.hpp>
#include "util.hxx"

class CompilationDatabaseCommand : public Request::CommandParser {
public:
  CompilationDatabaseCommand (const std::string & name, Application & application)
    : Request::CommandParser (name, "Read a compilation database"),
      application_ (application)
  {
    prompt_ = "load> ";
    defaults();

    using Request::key;
    add (key ("database", args_.fileName)
         ->metavar ("FILENAME")
         ->description ("Load compilation commands from a JSON compilation database"));
  }

  void defaults () {
    args_.fileName = "compile_commands.json";
  }

  void run (std::ostream & cout) {
    application_.compilationDatabase (args_, cout);
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

  void run (std::ostream & cout) {
    application_.update (args_, cout);
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

  void run (std::ostream & cout) {
    application_.index (args_, cout);
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
    add (key ("mostSpecific", args_.mostSpecific)
         ->metavar ("true|false")
         ->description ("Display only the most specific identifier at this location"));
    add (key ("diagnostics", args_.diagnostics)
         ->metavar ("true|false")
         ->description ("Print compilation diagnostics"));
    add (key ("fromIndex", args_.fromIndex)
         ->metavar ("true|false")
         ->description ("Search in the index (faster but potentially out-of-date)"));
  }

  void defaults () {
    args_.fileName = "";
    args_.offset = 0;
    args_.mostSpecific = false;
    args_.diagnostics = true;
    args_.fromIndex = true;
  }

  void run (std::ostream & cout) {
    application_.findDefinition (args_, cout);
  }

private:
  Application & application_;
  Application::FindDefinitionArgs args_;
};


class GrepCommand : public Request::CommandParser {
public:
  GrepCommand (const std::string & name, Application & application)
    : Request::CommandParser (name, "Find all references to a definition"),
      application_ (application)
  {
    prompt_ = "grep> ";
    defaults();

    using Request::key;
    add (key ("usr", args_.usr)
         ->metavar ("USR")
         ->description ("Unified Symbol Resolution for the symbol"));
  }

  void defaults () {
    args_.usr = "c:@F@main";
  }

  void run (std::ostream & cout) {
    application_.grep (args_, cout);
  }

private:
  Application & application_;
  Application::GrepArgs args_;
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
         ->metavar ("FILENAME")
         ->description ("Source file name"));
    add (key ("line", args_.line)
         ->metavar ("LINE_NO")
         ->description ("Line number (counting from 0)"));
    add (key ("column", args_.column)
         ->metavar ("COLUMN_NO")
         ->description ("Column number (counting from 0)"));
  }

  void defaults () {
    args_.fileName = "";
    args_.line = 0;
    args_.column = 0;
  }

  void run (std::ostream & cout) {
    application_.complete (args_, cout);
  }

private:
  Application & application_;
  Application::CompleteArgs args_;
};

struct ExitCommand : public Request::CommandParser {
  ExitCommand (const std::string & name)
    : Request::CommandParser (name, "Shutdown server")
  {
    prompt_ = "exit> ";
  }

  void run (std::ostream & cout) {
    throw std::runtime_error ("Exiting...");
  }
};


int main () {
  Storage storage;

  Application app (storage);

  Request::Parser p ("Clang-tags server\n");
  p .add (new CompilationDatabaseCommand ("load", app))
    .add (new IndexCommand ("index", app))
    .add (new UpdateCommand ("update", app))
    .add (new FindCommand ("find", app))
    .add (new GrepCommand ("grep", app))
    .add (new CompleteCommand ("complete", app))
    .add (new ExitCommand ("exit"))
    .prompt ("clang-dde> ");

  // p.parse (std::cin, std::cout);

  try
  {
    asio::io_service io_service;
    asio::local::stream_protocol::endpoint endpoint ("/tmp/clang-tags");
    asio::local::stream_protocol::acceptor acceptor (io_service, endpoint);
    for (;;)
    {
      asio::local::stream_protocol::iostream socket;
      asio::error_code err;
      acceptor.accept(*socket.rdbuf(), err);
      if (!err) {
        std::cerr << "Receiving client request:" << std::endl;

        std::stringstream request;
        while (true) {
          std::string line;
          std::getline (socket, line);
          if (line == "")
            break;

          std::cerr << line << std::endl;
          request << line << std::endl;
        }

        Json::Value json;
        request >> json;

        std::cerr << "Processing request... ";
        socket << "Server response:" << std::endl << std::flush;
        p.parseJson (json, socket);
        std::cerr << "done." << std::endl << std::endl;
      }
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_SUCCESS;
}
