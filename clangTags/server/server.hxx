#pragma once

#include "clangTags/server/load.hxx"
#include "clangTags/server/config.hxx"
#include "clangTags/server/index.hxx"
#include "clangTags/server/findDefinition.hxx"
#include "clangTags/server/grep.hxx"
#include "clangTags/server/complete.hxx"
#include "clangTags/server/exit.hxx"

#include "MT/stream.hxx"
#include <boost/asio.hpp>

namespace ClangTags {
namespace Server {
class Server {
public:
  Server (ClangTags::Cache & cache,
          ClangTags::Indexer::Indexer & indexer)
    : cache_      (cache),
      indexer_    (indexer),
      parser_     ("Clang-tags server\n"),
      pidPath_    (".ct.pid"),
      socketPath_ (".ct.sock")
  {
    std::ofstream pidFile (pidPath_);
    pidFile << getpid() << std::endl;
    pidFile.close();

    parser_
      .add (new ClangTags::Server::Load   (storage_, indexer_))
      .add (new ClangTags::Server::Config (storage_))
      .add (new ClangTags::Server::Index  (indexer_))
      .add (new ClangTags::Server::FindDefinition (storage_, cache_))
      .add (new ClangTags::Server::Grep   (storage_))
      .add (new ClangTags::Server::Complete (storage_, cache_))
      .add (new ClangTags::Server::Exit)
      .prompt ("clang-tags> ");
  }

  ~Server () {
    MT::cerr() << "Server exiting..." << std::endl;
    unlink (socketPath_.c_str());
    unlink (pidPath_.c_str());
  }

  void run (bool fromStdin) {
    if (fromStdin) {
      parser_.parseJson (std::cin, std::cout);
    }
    else {
      try {
        MT::cerr() << "Server starting with pid: " << getpid() << std::endl;

        boost::asio::io_service io_service;
        boost::asio::local::stream_protocol::endpoint endpoint (socketPath_);
        boost::asio::local::stream_protocol::acceptor acceptor (io_service, endpoint);
        for (;;) {
          boost::asio::local::stream_protocol::iostream socket;
          boost::system::error_code err;
          acceptor.accept(*socket.rdbuf(), err);
          if (!err) {
            parser_.parseJson (socket, socket, /*verbose=*/true);
          }
        }
      } catch (...) {}
    }
  }

private:
  ClangTags::Cache & cache_;
  ClangTags::Indexer::Indexer & indexer_;
  Storage storage_;
  Request::Parser parser_;
  std::string pidPath_;
  std::string socketPath_;
};
}
}
