#ifndef PDWORLD_HPP
#define PDWORLD_HPP

#include "PDworldHelpers.hpp"

class PDworld {
private:
  const double ALPHA;          // learning rate
  const double GAMMA;          // discount rate
  const int GRID_I;            // I size of PDworld
  const int GRID_J;            // J size of PDworld
  const PDstate initialState;  // the initial state for all experiments
  const PDstate terminalState; // the terminal state for all experiments
  const Rewards rewards;       // rewards for different actions defined

public:
  PDworld(const PDstate initialState, const PDstate terminalState,
          const Rewards rewards, const double ALPHA, const double GAMMA,
          const int GRID_I, const int GRID_J);
};

#endif
