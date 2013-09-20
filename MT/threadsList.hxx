#pragma once

#include <list>
#include <boost/thread/thread.hpp>

namespace MT {
/** @brief List of threads
 *
 * This object stores a collection of @c boost::thread objects. The ThreadsList
 * destructor interrupts all threads and waits for their termination.
 */
class ThreadsList {
public:
  /** @brief Interrupt and join threads
   *
   * On ThreadsList destruction, all threads are interrupt()-ed and join()-ed.
   */
  ~ThreadsList () {
    for (auto it = threads_.begin() ; it != threads_.end() ; ++it)
      it->interrupt();
    for (auto it = threads_.begin() ; it != threads_.end() ; ++it)
      it->join();
  }

  /** @brief Create a thread and start managing it
   *
   * This function takes the same kind of argument as the boost::thread
   * constructor.
   */
  template <typename T>
  void add (T & function) {
    threads_.push_back (boost::thread (function));
  }

private:
  std::list<boost::thread> threads_;
};
}
