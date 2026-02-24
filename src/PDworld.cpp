#include "PDworld.hpp"
#include <iostream>

PDworld::PDworld(int i, int j, bool x, int a, int b, int c, int d, int e, int f,
                 int gridX, int gridY, int ti, int tj, bool tx, int ta, int tb,
                 int tc, int td, int te, int tf)
    : state(StateWorld(i, j, x, a, b, c, d, e, f, ti, tj, tx, ta, tb, tc, td,
                       te, tf)),
      gridX(gridX), gridY(gridY) {
  if (i > gridX || j > gridY) {
    throw std::invalid_argument("gridX or gridY does not abide with i, or j");
  }
};
