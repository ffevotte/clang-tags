#pragma once

#include <iostream>
#include <boost/thread.hpp>

namespace MT {
class OStream {
public:
  typedef boost::mutex Mutex;

  OStream (std::ostream & cout)
    : cout_ (cout)
  {}

  ~OStream () {}

  class Guard {
  public:
    class Impl {
    public:
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

    private:
      boost::lock_guard<Mutex> guard_;
      std::ostream & cout_;
    };

    Guard (std::ostream & cout, Mutex & mtx)
      : impl_ (new Impl (cout, mtx))
    {}

    ~Guard () {
      delete impl_;
    }

    template <typename T>
    Impl & operator<< (const T& x) {
      return (*impl_) << x;
    }

    Impl & operator<< (std::ostream & (*manip) (std::ostream&)) {
      return (*impl_) << manip;
    }

  private:
    Impl * impl_;
  };

  Guard operator() () {
    return Guard (cout_, mtx_);
  }


private:
  Mutex mtx_;

  std::ostream & cout_;
};

extern OStream cerr;
}
