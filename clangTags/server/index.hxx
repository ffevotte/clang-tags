#pragma once

#include "request/request.hxx"

#include "clangTags/indexer/indexer.hxx"

namespace ClangTags {
namespace Server {

class Index : public Request::CommandParser {
public:
  Index (Indexer::Indexer & indexer);
  void run (std::ostream & cout);

protected:
  Indexer::Indexer & indexer_;
};

}
}
