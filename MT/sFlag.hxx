#pragma once

#include <boost/thread.hpp>

namespace MT {
template <typename T>
class SFlag {
public:
  SFlag (const T & val)
    : val_ (val)
  {}

  void set (const T & val) {
    {
      boost::lock_guard<boost::mutex> lock (mtx_);
      val_  = val;
      flag_ = true;
    }
    cond_.notify_one();
  }

  T get () {
    boost::unique_lock<boost::mutex> lock (mtx_);
    flag_ = false;
    while (!flag_) {
      cond_.wait(lock);
    }
    return val_;
  }

private:
  boost::condition_variable cond_;
  boost::mutex mtx_;
  bool flag_;
  T val_;
};
}
