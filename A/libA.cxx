#include "libA.hxx"
#include <iostream>

A::A () {}

void A::display () const {
  std::cout << "Hello, I'm an A!" << std::endl;
}
