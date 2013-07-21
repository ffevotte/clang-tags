#include "config.h"
#include <iostream>

#ifdef DEBUG
inline void debug (const char* message) {
  std::cerr << message << std::endl;
}
#else
inline void debug (const char* message) {}
#endif

template <typename T> struct MyClass {
  MyClass () { debug ("MyClass::MyClass"); }

  void display () {
    std::cout << "MyClass<T>::display()" << std::endl;
  }
};

template <> struct MyClass<int> {
  void display () {
    std::cout << "MyClass<int>::display()" << std::endl;
  }
};

int main () {
  int display = 3;

  MyClass<double> a;
  a.display();

  MyClass<int> b;
  b.display();
}
