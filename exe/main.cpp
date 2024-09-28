#include "oblivion/ordered_list.hpp"
#include <iostream>

int main() {
  std::cout << "oblivion v0.1.0" << std::endl;

  oblivion::OrderedList list;
  list.include(1);
  list.include(0);
  list.include(2);
  list.include(20);
  list.include(5);
  list.include(3);
  list.include(4);
  list.include(40);
  list.include(6);

  std::cout << list << std::endl;

  list.erase(4);
  list.erase(1);
  list.erase(5);

  std::cout << list << std::endl;

  return 0;
}
