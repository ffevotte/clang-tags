#pragma once

#include <iostream>
#include <boost/thread.hpp>

namespace MT {
class OStream {
public:
  OStream (std::ostream & cout)
    : guard_ (mtx_),
      cout_ (cout)
  {}

  ~OStream () {}

  template <typename T>
  OStream & operator<< (const T& x) {
    cout_ << x;
    return *this;
  }

  OStream & operator<< (std::ostream & (*manip) (std::ostream&)) {
    cout_ << manip;
    return *this;
  }

private:
  typedef boost::mutex Mutex;
  typedef boost::lock_guard<Mutex> Guard;
  static  Mutex mtx_;
  Guard guard_;

  std::ostream & cout_;
};
}
