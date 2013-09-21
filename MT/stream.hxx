#pragma once

#include <iostream>
#include <boost/thread.hpp>

namespace MT {
/** @addtogroup mt
 *  @{
 */

/** @brief Output stream
 *
 * This class provides output streams protected against concurrent write
 * operations in multi-threaded environments.
 */
class OStream {
public:
  /** @brief mutex type used to protect the streams
   */
  typedef boost::mutex Mutex;

  /** @brief Constructor
   *
   * @param cout  underlying output stream
   */
  OStream (std::ostream & cout)
    : cout_ (cout)
  {}

  ~OStream () {}

  /** @brief Protected output stream proxy
   *
   * This is class implements the output stream concept, and is the one actually
   * used to output to the underlying stream.
   */
  class Proxy {
  public:
    ~Proxy ();

    /** @brief Output operator
     *
     * @param x  object to output
     * @return @c *this
     */
    template <typename T>
    Proxy & operator<< (const T& x);

    /** @brief Output operator
     *
     * @param manip manipulator
     * @return @c *this
     */
    Proxy & operator<< (std::ostream & (*manip) (std::ostream&));

  private:
    friend class OStream;
    Proxy (std::ostream & cout, Mutex & mtx);

    class Impl;
    Impl * impl_;
  };

  /** @brief Acquire a locked output stream proxy
   *
   * As long as the object returned by this method lives, it will be the only
   * one allowing output to the underlying stream.
   *
   * @return a locked output stream proxy
   */
  Proxy operator() () {
    return Proxy (cout_, mtx_);
  }


private:
  Mutex mtx_;
  std::ostream & cout_;
};

/** @brief Protected error output stream */
extern OStream cerr;

/** @} */


/** @brief Opaque internal type
 */
class OStream::Proxy::Impl {
private:
  friend class OStream::Proxy;

  Impl (std::ostream & cout, Mutex & mtx)
    : guard_ (mtx),
      cout_ (cout)
  {}

  template <typename T>
  Impl & operator<< (const T& x) {
    cout_ << x;
    return *this;
  }

  Impl & operator<< (std::ostream & (*manip) (std::ostream&)) {
    cout_ << manip;
    return *this;
  }

  boost::lock_guard<Mutex> guard_;
  std::ostream & cout_;
};

inline
OStream::Proxy::Proxy (std::ostream & cout, Mutex & mtx)
  : impl_ (new Impl (cout, mtx))
{}

inline
OStream::Proxy::~Proxy () {
  delete impl_;
}

template <typename T>
OStream::Proxy & OStream::Proxy::operator<< (const T& x) {
  (*impl_) << x;
  return *this;
}

inline
OStream::Proxy & OStream::Proxy::operator<< (std::ostream & (*manip) (std::ostream&)) {
  (*impl_) << manip;
  return *this;
}
}
