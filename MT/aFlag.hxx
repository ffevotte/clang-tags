#pragma once

#include <boost/thread.hpp>

namespace MT {
template <typename T>
class AFlag {
public:
  AFlag (const T& val)
    : val_ (val)
  { }

  void set (const T& val) {
    Guard _ (mtx_);
    val_ = val;
  }

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
}
