#include "PDworld.hpp"
#include <iostream>

PDworld::PDworld(const PDstate initial, const PDstate terminal,
                 const Rewards rewards, const double ALPHA, const double GAMMA,
                 const int GRID_I, const int GRID_J)
    : initialState(initialState), terminalState(terminalState),
      rewards(rewards), ALPHA(ALPHA), GAMMA(GAMMA), GRID_I(GRID_I),
      GRID_J(GRID_J) {}
