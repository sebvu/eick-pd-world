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
  bool onDropoffE = isAt({s.e_loc.first, s.e_loc.second});
  bool onDropoffF = isAt({s.f_loc.first, s.f_loc.second});
  if (s.x == true && (onDropoffD || onDropoffE || onDropoffF))
    ops.push_back(Action::Dropoff);

  return ops;
}

// changes world state given action, returns the reward given the change
int PDworld::apply(PDstate &s, const Action a) {

  std::pair<int, int> p = {s.i, s.j};

  switch (a) {
  case Action::North:
    s.j--;
    return PDworld::rewards.nReward;
    break;
  case Action::East:
    s.i++;
    return PDworld::rewards.eReward;
    break;
  case Action::South:
    s.j++;
    return PDworld::rewards.sReward;
    break;
  case Action::West:
    s.i--;
    return PDworld::rewards.wReward;
    break;
  case Action::Pickup: {
    (*s.loc_val[p])--;
    return PDworld::rewards.pickupReward;
    break;
  }
  case Action::Dropoff: {
    (*s.loc_val[p])++;
    return PDworld::rewards.dropoffReward;
    break;
  }
  }
}

void PDworld::QLearning(std::vector<std::pair<int, std::string>> instructions) {

}

void PDworld::SARSA(std::vector<std::pair<int, std::string>> instructions) {}
