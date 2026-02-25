#ifndef PDWORLD_HPP
#define PDWORLD_HPP

#include "PDworldHelpers.hpp"
#include <array>
#include <vector>

class PDworld {
private:
  const double ALPHA;                   // learning rate
  const double GAMMA;                   // discount rate
  static constexpr int GRID_I = 5;      // I size of PDworld
  static constexpr int GRID_J = 5;      // J size of PDworld
  static constexpr int NUM_ACTIONS = 6; // set number of actions
  const PDstate initialState;           // the initial state for all experiments
  const PDstate terminalState; // the terminal state for all experiments
  const Rewards rewards;       // rewards for different actions defined

  // the 2 is simply X as 0 or 1
  using QTable = std::array<
      std::array<std::array<std::array<double, NUM_ACTIONS>, 2>, GRID_I>,
      GRID_J>;

  QTable Q{}; // the actual Q-table initialized, all values as 0

  // different policies
  static constexpr std::string_view PRANDOM = "PRANDOM";
  static constexpr std::string_view PEXPLOIT = "PEXPLOIT";
  static constexpr std::string_view PGREEDY = "PGREEDY";
  static constexpr std::string_view DISPLAY = "DISPLAY"; // display the Q-table

  enum class Action : int {
    North = 0,
    East = 1,
    South = 2,
    West = 3,
    Pickup = 4,
    Dropoff = 5
  };

  // return a set of applicable operations
  std::vector<Action> aplop(const PDstate &s) {
    std::vector<Action> ops;

    // verify a cardinal direction
    if (s.i > 1)
      ops.push_back(Action::West);
    if (s.i < GRID_I)
      ops.push_back(Action::East);
    if (s.j > 1)
      ops.push_back(Action::North);
    if (s.j < GRID_J)
      ops.push_back(Action::South);

    // lambda function to verify if s.i and s.j is in a *_loc pos
    auto isAt = [&](std::pair<int, int> loc) {
      return s.i == loc.first && s.j == loc.second;
    };

    // verify if pickup is valid
    bool onPickupA = isAt({s.a_loc.first, s.a_loc.second});
    bool onPickupB = isAt({s.b_loc.first, s.b_loc.second});
    bool onPickupC = isAt({s.c_loc.first, s.c_loc.second});
    if (s.x == false && (onPickupA || onPickupB || onPickupC))
      ops.push_back(Action::Pickup);

    // verify if dropoff is valid
    bool onDropoffD = isAt({s.d_loc.first, s.d_loc.second});
    bool onDropoffE = isAt({s.e_loc.first, s.d_loc.second});
    bool onDropoffF = isAt({s.f_loc.first, s.f_loc.second});
    if (s.x == true && (onDropoffD || onDropoffE || onDropoffF))
      ops.push_back(Action::Dropoff);

    return ops;
  }

public:
  PDworld(const PDstate *initialState, const PDstate *terminalState,
          const Rewards *rewards, const double ALPHA, const double GAMMA);

  void QLearning(std::vector<std::pair<int, std::string>> instructions);

  void SARSA(std::vector<std::pair<int, std::string>> instructions);
};

#endif
