#pragma once

#include <boost/thread.hpp>

namespace MT {
/** @addtogroup mt
 *  @{
 */

/** @brief Asynchronous flag
 *
 * Flag used to exchange data between threads in an asynchronous way.
 *
 * @tparam T  type of data
 */
template <typename T>
class AFlag {
public:
  /** @brief Constructor
   *
   * @param val  initial value
   */
  AFlag (const T& val)
    : val_ (val)
  { }

  /** @brief Set the flag value
   *
   * @param val  new flag value
   */
  void set (const T& val) {
    Guard _ (mtx_);
    val_ = val;
  }

  /** @brief Retrieve the flag value
   *
   * This is a non-blocking operation. The current flag value is returned
   * immediately.
   *
   * @return current flag value
   */
  T& get () {
    Guard _ (mtx_);
    return val_;
  }

private:
  typedef boost::mutex Mutex;
  typedef boost::lock_guard<Mutex> Guard;
  Mutex mtx_;
  T val_;
};
/** @} */
}
