#include "PDworld.hpp"
#include <iostream>

PDworld::PDworld(const PDstate initialState, const PDstate terminalState,
                 const Rewards rewards, const double ALPHA, const double GAMMA,
                 const int GRID_I, const int GRID_J)
    : ALPHA(ALPHA), GAMMA(GAMMA),
      GRID_I(GRID_I), GRID_J(GRID_J), initialState(initialState), terminalState(terminalState),
      rewards(rewards) {}
