#include "PDworld.hpp"
#include <iostream>

int main() {

  // initial state:(1,5,false,5,5,5,0,0,0)
  // terminal state: (*,*,false,0,0,0,5,5,5)
  // (1, 5, false, 5, 5, 5, 0, 0, 0, (skipping *), false, .... etc)

  PDworld world = PDworld(1, 5, false, 5, 5, 5, 0, 0, 0, 5, 5, false, 0, 0, 0,
                          5, 5, 5, 13, 13, -1, -1, -1, -1);
}
