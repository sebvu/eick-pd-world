#include "PDworld.hpp"
#include "PDworldHelpers.hpp"
#include <vector>

#define PRANDOM "PRANDOM"
#define PEXPLOIT "PEXPLOIT"
#define PGREEDY "PGREEDY"
#define DISPLAY "DISPLAY"

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

  const std::pair<int, int> a_loc = {1, 1}, b_loc = {3, 3}, c_loc = {5, 5},
                            d_loc = {5, 1}, e_loc = {5, 3}, f_loc = {2, 5};

  const PDstate initialState{
      .i = 1,
      .j = 5,
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

  PDworld world =
      PDworld(&initialState, &terminalState, &rewards, ALPHA, GAMMA);

  // format of sets for experiment instructions

  // (set<pair<int, string>>({STEPS, POLICY} ... or ... {STEPS, DISPLAY})
  //  - STEPS are added on top of each other
  //  - POLICY is just defining what policy to do till that defined step for the
  //  algorithm
  //  - display is simply displaying the Q-table for the experiment
  //    - the number is completely redundant for display
  //  - amount of steps is the STEPS combined

  // Q-learning
  std::vector<std::pair<int, std::string>> experimentOneInstructions = {
      {4000, PRANDOM},
      {69696942067676767, DISPLAY},
      {4000, PGREEDY},
      {69694206767, DISPLAY}};

  // Q-learning
  std::vector<std::pair<int, std::string>> experimentTwoInstructions = {
      {200, PRANDOM},
      {676767, DISPLAY},
      {3000, PEXPLOIT},
      {6767676767, DISPLAY},
      {4800, PEXPLOIT}};

  // SARSA
  std::vector<std::pair<int, std::string>> experimentThreeInstructions = {
      {200, PRANDOM}, {7800, PEXPLOIT}, {6767676767767, DISPLAY}};

  world.QLearning(experimentOneInstructions, 1);

  world.QLearning(experimentTwoInstructions, 2);

  world.SARSA(experimentThreeInstructions, 3);
}
