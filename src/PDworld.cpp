#include "PDworld.hpp"
#include <iostream>

PDworld::PDworld(const PDstate initialState, const PDstate terminalState,
                 const Rewards rewards, const double ALPHA, const double GAMMA)
    : ALPHA(ALPHA), GAMMA(GAMMA), initialState(initialState),
      terminalState(terminalState), rewards(rewards) {}

void PDworld::QLearning(std::set<std::pair<int, std::string>> instructions) {}

void PDworld::SARSA(std::set<std::pair<int, std::string>> instructions) {}
