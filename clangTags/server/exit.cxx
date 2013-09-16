#include "exit.hxx"
#include <stdexcept>

namespace ClangTags {
namespace Server {
Exit::Exit ()
  : Request::CommandParser ("exit", "Shutdown server")
{}

void Exit::run (std::ostream & cout) {
  cout << "Exiting..." << std::endl;
  throw std::runtime_error ("shutdown requested");
}
}
}
