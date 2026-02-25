#ifndef PDWORLD_HPP
#define PDWORLD_HPP

#include "PDworldHelpers.hpp"
#include <array>
#include <string>
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
  using Qtable = std::array<
      std::array<
          std::array<
              std::array<std::array<std::array<std::array<double, NUM_ACTIONS>,
                                               2>, // u
                                    2>,            // t
                         2>,                       // s
              2>,                                  // x
          GRID_J>,                                 // j
      GRID_I>;                                     // i

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
  std::vector<Action> aplop(const PDstate &s);

  // apply the world state given action, return reward given the change
  double apply(PDstate &s, const Action a);

  // get the requsted Q value
  double getQUtil(PDstate &s, Qtable &q, Action a);

  // run the QLearning algorithm
  double getQLearningUtility(const double r, const double Qas, double maxQas);

  // run the SARSA algorithm
  double getSARSAUtility(const double r, const double Qas,
                         const double nextQas);

  // PRANDOM policy
  Action getOperationWithPRANDOM(std::vector<Action> &ops);

  // PGREEDY policy
  Action getOperationWithPGREEDY(std::vector<Action> &ops, PDstate &s,
                                 Qtable &q);

  // PEXPLOIT policy
  Action getOperationWithPEPLOIT(std::vector<Action> &ops, PDstate &s,
                                 Qtable &q);

public:
  PDworld(const PDstate *initialState, const PDstate *terminalState,
          const Rewards *rewards, const double ALPHA, const double GAMMA);

  void QLearning(std::vector<std::pair<int, std::string>> instructions);

  void SARSA(std::vector<std::pair<int, std::string>> instructions);
};

#endif
