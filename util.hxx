#pragma once

#include <sys/time.h>

class Timer {
public:
  Timer () {
    reset();
  }

  double get () {
    struct timeval now;
    gettimeofday (&now, NULL);

    return now.tv_sec - start_.tv_sec + 1e-6 * (now.tv_usec - start_.tv_usec);
  }

  void reset () {
    gettimeofday (&start_, NULL);
  }

private:
  struct timeval start_;
};


struct String : public std::string {
  String (const std::string & other)
    : std::string (other)
  {}

  bool startsWith (const std::string & prefix) const {
    return compare(0, prefix.size(), prefix) == 0;
  }
};


class Tee : public std::ostream {
public:
  Tee (std::ostream & stream1,
       std::ostream & stream2)
    : stream1_ (stream1),
      stream2_ (stream2)
  { }

  template <typename T>
  Tee & operator<< (T x) {
    stream1_ << x;
    stream2_ << x;
    return *this;
  }

  Tee & operator<< (std::ostream & (*x) (std::ostream&)) {
    stream1_ << x;
    stream2_ << x;
    return *this;
  }


private:
  std::ostream & stream1_;
  std::ostream & stream2_;
};
