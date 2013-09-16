#include "index.hxx"

namespace ClangTags {
namespace Server {

Index::Index (Indexer::Indexer & indexer)
  : Request::CommandParser ("index", "Update the source code index"),
    indexer_ (indexer)
{
  prompt_ = "index> ";
}

void Index::run (std::ostream & cout) {
  indexer_.index();
  cout << "Waiting for the index to be rebuilt..." << std::endl;
  indexer_.wait();
  cout << "Done." << std::endl;
}

}
}
