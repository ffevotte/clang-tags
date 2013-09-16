#include "load.hxx"
#include "json/json.h"

#include <fstream>
#include <sstream>
#include <unistd.h>
#include <stdexcept>

namespace ClangTags {
namespace Server {

Load::Load (Storage & storage,
            Indexer::Indexer    & indexer)
  : Request::CommandParser ("load", "Read a compilation database"),
    storage_ (storage),
    indexer_ (indexer)
{
  prompt_ = "load> ";
  defaults();

  using Request::key;
  add (key ("database", args_.fileName)
       ->metavar ("FILEPATH")
       ->description ("Load compilation commands from a JSON compilation database"));

  const size_t size = 4096;
  cwd_ = new char[size];
  if (getcwd (cwd_, size) == NULL) {
    // FIXME: correctly handle this case
    throw std::runtime_error ("Not enough space to store current directory name.");
  }
}

Load::~Load () {
  delete[] cwd_;
}

void Load::defaults () {
  args_.fileName = "compile_commands.json";
}

void Load::run (std::ostream & cout) {
  // Change back to the original WD (in case `index` or `update` would have
  // changed it)
  chdir (cwd_);

  Json::Value root;
  Json::Reader reader;

  std::ifstream json (args_.fileName);
  bool ret = reader.parse (json, root);
  if ( !ret ) {
    cout  << "Failed to parse compilation database `" << args_.fileName << "'\n"
          << reader.getFormattedErrorMessages();
  }

  for (unsigned int i=0 ; i<root.size() ; ++i) {
    std::string fileName = root[i]["file"].asString();
    std::string directory = root[i]["directory"].asString();

    std::vector<std::string> clArgs;
    std::istringstream command (root[i]["command"].asString());
    std::string arg;
    std::getline (command, arg, ' ');
    do {
      std::getline (command, arg, ' ');
      clArgs.push_back (arg);
    } while (! command.eof());

    cout << "  " << fileName << std::endl;
    storage_.setCompileCommand (fileName, directory, clArgs);
  }

  indexer_.index();
}
}
}
