#pragma once

#include <list>
#include <boost/thread/thread.hpp>

namespace MT {
/** @addtogroup mt
 *  @{
 */

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
   * constructor:
   * - @c Callable must be copyable
   * - @c function() must be a valid expression
   *
   * The provided functor is copied and invoked in a newly created thread, which
   * is put into the list of managed threads.
   *
   * Use @c boost::ref to pass a reference to the functor object if it must not
   * be copied.
   *
   * @param functor  functor invoked in the newly created thread.
   */
  template <typename Callable>
  void add (Callable & functor) {
    threads_.push_back (boost::thread (functor));
  }

private:
  std::list<boost::thread> threads_;
};
/** @} */
}
