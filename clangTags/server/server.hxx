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
/** @addtogroup clangTags
 *  @{
 */

/** @brief Handles user requests
 *
 * This class implements a server handling user requests using the @ref request
 * module. Requests can either be read from the command-line or from a UNIX
 * socket (see @ref run).
 */
class Server {
public:
  /** @brief Constructor
   *
   * This constructor stores the server @c PID in a pidfile, in order to avoid
   * starting multiple servers for the same project.
   *
   * @param cache    @ref Cache instance from where translation units can be
   *                 retrieved
   *
   * @param indexer  @ref Indexer instance to be used for updating the source
   *                 files index
   */
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

  /** @brief Destructor
   *
   * Removes the pid and socket files.
   */
  ~Server () {
    MT::cerr() << "Server exiting..." << std::endl;
    unlink (socketPath_.c_str());
    unlink (pidPath_.c_str());
  }

  /** @brief Main server loop
   *
   * If the server listens on the standard input stream, it returns after having
   * handled one request.

   * Otherwise, a full server listening on a UNIX socket is run. It handles all
   * requests, until receiving an exception.
   *
   * @param fromStdin  If @c true, read requests from the standard input stream.
   *                   Otherwise, listen on a UNIX socket.
   */
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
/** @} */
}
}
