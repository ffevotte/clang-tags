#include "config.h"
#include <iostream>

#ifdef DEBUG
inline void debug (const char* message) {                //(ref:debug1)
  std::cerr << message << std::endl;
}
#else
inline void debug (const char* message) {}               //(ref:debug2)
#endif

template <typename T> struct MyClass {
  MyClass () { debug ("MyClass::MyClass"); }             //(ref:defCtor)

  void display () {                                      //(ref:defDisplayT)
    std::cout << "MyClass<T>::display()" << std::endl;
  }
};

template <> struct MyClass<int> {
  void display () {                                      //(ref:defDisplayInt)
    std::cout << "MyClass<int>::display()" << std::endl;
  }
};

int main () {
  int display = 3;                                       //(ref:display1)

  MyClass<double> a;                                     //(ref:ctor)
  a.display();                                           //(ref:display2)

  MyClass<int> b;
  b.display();                                           //(ref:display3)
}
