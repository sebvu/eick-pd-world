#include "PDworld.hpp"
#include "PDworldHelpers.hpp"
#include <iostream>

int main() {

  /* initial state: (1,5,0,5,5,5,0,0,0)
  terminal state: (*,*,0,0,0,0,5,5,5)
  alpha and gamma will always be the same
  this world will always be a 5x5 grid
  a, b, c, d, e, f must have specific location pairs defined

  THIS PDWORLD WILL BE REPRESENTED IN A 1 INDEXED PARADIGM
  */

  constexpr double ALPHA = 0.3; // learning rate
  constexpr double GAMMA = 0.5; // discount rate
  constexpr int GRID_I = 5;     // I size of PDworld
  constexpr int GRID_J = 5;     // J size of PDworld

  const std::pair<int, int> a_loc = {1, 1}, b_loc = {3, 3}, c_loc = {5, 5},
                            d_loc = {5, 1}, e_loc = {5, 3}, f_loc = {2, 5};

  const PDstate initialState{
      .i = 1,
      .j = 5,
      .x = false,
      .a = 5,
      .b = 5,
      .c = 5,
      .d = 0,
      .e = 0,
      .f = 0,

      .a_loc = a_loc,
      .b_loc = b_loc,
      .c_loc = c_loc,

      .d_loc = d_loc,
      .e_loc = e_loc,
      .f_loc = f_loc,
  };

  const PDstate terminalState{
      .x = false,
      .a = 0,
      .b = 0,
      .c = 0,
      .d = 5,
      .e = 5,
      .f = 5,

      .a_loc = a_loc,
      .b_loc = b_loc,
      .c_loc = c_loc,

      .d_loc = d_loc,
      .e_loc = e_loc,
      .f_loc = f_loc,
  };

  const Rewards rewards{
      .pickupReward = 13,
      .dropoffReward = 13,
      .nReward = -1,
      .sReward = -1,
      .eReward = -1,
      .wReward = -1,
  };

  // actual PDworld initializaiton and tests

  PDworld world = PDworld(initialState, terminalState, rewards, ALPHA, GAMMA,
                          GRID_I, GRID_J);
}
