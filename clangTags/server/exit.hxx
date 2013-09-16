#pragma once

#include "request/request.hxx"

namespace ClangTags {
namespace Server {

class Exit : public Request::CommandParser {
public:
  Exit ();
  void run (std::ostream & cout);
};
}
}
