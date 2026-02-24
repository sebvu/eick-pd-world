#ifndef PDWORLD_HPP
#define PDWORLD_HPP

#include "PDworldHelpers.hpp"
#include <set>

class PDworld {
private:
  const double ALPHA;          // learning rate
  const double GAMMA;          // discount rate
  const int GRID_I;            // I size of PDworld
  const int GRID_J;            // J size of PDworld
  const PDstate initialState;  // the initial state for all experiments
  const PDstate terminalState; // the terminal state for all experiments
  const Rewards rewards;       // rewards for different actions defined

  // different policies
  static constexpr std::string_view PRANDOM = "PRANDOM";
  static constexpr std::string_view PEXPLOIT = "PEXPLOIT";
  static constexpr std::string_view PGREEDY = "PGREEDY";
  static constexpr std::string_view DISPLAY = "DISPLAY"; // display the Q-table

  static constexpr std::string_view NDIR = "NORTH";      // display the Q-table
  static constexpr std::string_view EDIR = "EAST";       // display the Q-table
  static constexpr std::string_view SDIR = "SOUTH";      // display the Q-table
  static constexpr std::string_view WDIR = "WEST";       // display the Q-table
  static constexpr std::string_view DROPOFF = "DROPOFF"; // display the Q-table
  static constexpr std::string_view PICKUP = "PICKUP";   // display the Q-table

public:
  PDworld(const PDstate initialState, const PDstate terminalState,
          const Rewards rewards, const double ALPHA, const double GAMMA,
          const int GRID_I, const int GRID_J);

  void QLearning(std::set<std::pair<int, std::string>> instructions);

  void SARSA(std::set<std::pair<int, std::string>> instructions);
};

#endif
