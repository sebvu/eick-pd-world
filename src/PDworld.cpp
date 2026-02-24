#include "PDworld.hpp"
#include <iostream>

PDworld::PDworld(int i, int j, bool x, int a, int b, int c, int d, int e, int f,
                 int gridX, int gridY)
    : state(StateWorld(i, j, x, a, b, c, d, e, f)), gridX(gridX), gridY(gridY) {
  if (i > gridX || j > gridY) {
    throw std::invalid_argument("gridX or gridY does not abide with i, or j");
  }
};
