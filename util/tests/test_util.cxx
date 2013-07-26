/** @example test_util.cxx
 *
 * This example shows how to use the classes of the @ref util module
 */
#include "util/util.hxx"
#include <sstream>

void check (bool expr) {
  if (!expr) {
    throw std::string ("Error");
  }
}


void testTimer () {
  //![Timer]
  Timer timer;

  // do something
  std::cout << "Testing Timer..." << std::endl;

  std::cout << "Elapsed time: " << timer.get() << " s." << std::endl;
  //![Timer]
}


void testString () {
  std::cout << "Testing String..." << std::endl;

  // Usage example
  //![String]
  String s ("foobar");

  check (s.startsWith ("foo"));
  //![String]


  // Additional tests
  check (s.startsWith ("bar") == false);
}


void testTee () {
  std::cout << "Testing Tee" << std::endl;

  //![Tee]
  std::stringstream stream;
  Tee tee (std::cout, stream);
  tee << "foo" << std::endl;

  std::cout << stream.str() << std::endl;
  //![Tee]
}


int main () {
  try {
    testTimer();
    testString();
    testTee();
  }
  catch (...) {
    std::cerr << "Caught exception!" << std::endl;
    return 1;
  }

  return 0;
}
