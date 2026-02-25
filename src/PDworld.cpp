#include "PDworld.hpp"
#include <vector>

PDworld::PDworld(const PDstate *initialState, const PDstate *terminalState,
                 const Rewards *rewards, const double ALPHA, const double GAMMA)
    : ALPHA(ALPHA), GAMMA(GAMMA), initialState(*initialState),
      terminalState(*terminalState), rewards(*rewards) {}

std::vector<PDworld::Action> PDworld::aplop(const PDstate &s) {
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

// only apply changes to the world state, not the Q table as that is applied by
// the algorithm
void PDworld::apply(PDstate &s, const Action a) {

  std::pair<int, int> p = {s.i, s.j};

  switch (a) {
  case Action::North:
    s.j--;
    break;
  case Action::East:
    s.i++;
    break;
  case Action::South:
    s.j++;
    break;
  case Action::West:
    s.i--;
    break;
  case Action::Pickup: {
    (*s.loc_val[p])--;
    break;
  }
  case Action::Dropoff: {
    (*s.loc_val[p])++;
    break;
  }
  }
}

void PDworld::QLearning(std::vector<std::pair<int, std::string>> instructions) {
}

void PDworld::SARSA(std::vector<std::pair<int, std::string>> instructions) {}
