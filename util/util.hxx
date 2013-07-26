#pragma once

#include <sys/time.h>
#include <iostream>

/** @defgroup util Utilities
 *  @brief Various utilities
 *
 *  @{
 */

/** @brief Wall-clock timer
 *
 * Example use:
 * @snippet test_util.cxx Timer
 */
class Timer {
public:
  /** @brief Constructor
   *
   * Create a new timer and start counting.
   */
  Timer () {
    reset();
  }

  /** @brief Get elapsed (wall-clock) time
   *
   * Time is counted since the timer creation or last reset().
   *
   * @return elapsed time, in seconds
   */
  double get () {
    struct timeval now;
    gettimeofday (&now, NULL);

    return now.tv_sec - start_.tv_sec + 1e-6 * (now.tv_usec - start_.tv_usec);
  }

  /** @brief Reset timer
   *
   * Start counting time from the current instant on.
   */
  void reset () {
    gettimeofday (&start_, NULL);
  }

private:
  struct timeval start_;
};


/** @brief Utility std::string subclass
 *
 * String provides all features of std::string, and more.
 *
 * Example use:
 * @snippet test_util.cxx String
 */
struct String : public std::string {
  /** @brief Constructor from std::string
   *
   * @param other  existing std::string to convert to String
   */
  String (const std::string & other)
    : std::string (other)
  {}

  /** @brief Tell whether the string begins with a prefix
   *
   * @param prefix  prefix string to be tested
   *
   * @return @c true iff the string begins with @c prefix
   */
  bool startsWith (const std::string & prefix) const {
    return compare(0, prefix.size(), prefix) == 0;
  }
};


/** @brief Output stream duplicator
 *
 * Stream which duplicates its output to two streams.
 *
 * Example use:
 * @snippet test_util.cxx Tee
 */
class Tee {
public:
  /** @brief Constructor
   *
   * @param stream1  first stream to duplicate output to
   * @param stream2  second stream to duplicate output to
   */
  Tee (std::ostream & stream1,
       std::ostream & stream2)
    : stream1_ (stream1),
      stream2_ (stream2)
  { }

  /** @brief Output stream operator
   *
   * Duplicate output to the two provided streams.
   *
   * @param x  thing to ouput
   *
   * @return  the tee itself
   */
  template <typename T>
  Tee & operator<< (T x) {
    stream1_ << x;
    stream2_ << x;
    return *this;
  }

  /** @brief Output stream operator
   *
   * Duplicate output to the two provided streams.
   *
   * @param x  stream manipulator to forward
   *
   * @return  the tee itself
   */
  Tee & operator<< (std::ostream & (*x) (std::ostream&)) {
    stream1_ << x;
    stream2_ << x;
    return *this;
  }

private:
  std::ostream & stream1_;
  std::ostream & stream2_;
};

/** @} */
