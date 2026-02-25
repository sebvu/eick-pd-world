#include "PDworld.hpp"
#include <random>
#include <stdexcept>
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
  if (s.x == 0 && (onPickupA || onPickupB || onPickupC))
    ops.push_back(Action::Pickup);

  // verify if dropoff is valid
  bool onDropoffD = isAt({s.d_loc.first, s.d_loc.second});
  bool onDropoffE = isAt({s.e_loc.first, s.e_loc.second});
  bool onDropoffF = isAt({s.f_loc.first, s.f_loc.second});
  if (s.x == 1 && (onDropoffD || onDropoffE || onDropoffF))
    ops.push_back(Action::Dropoff);

  return ops;
}

// changes world state given action, returns the reward given the change
double PDworld::apply(PDstate &s, const Action a) {

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
    s.cellRef({s.i, s.j})--;
    s.x = 1;
    return rewards.pickupReward;
  }
  case Action::Dropoff: {
    s.cellRef({s.i, s.j})++;
    s.x = 0;
    return rewards.dropoffReward;
  }
  default:
    throw std::logic_error("PDworld::apply: unknown Action enum value");
  }
}
double PDworld::getQUtil(PDstate &s, Qtable &q, Action a) {
  int s0 = 0, t0 = 0, u0 = 0;

  // not carrying
  if (s.x == 0) {
    if (s.cellRef(s.a_loc) >= 1)
      s0 = 1;
    if (s.cellRef(s.b_loc) >= 1)
      t0 = 1;
    if (s.cellRef(s.c_loc) >= 1)
      u0 = 1;
  }
  // carrying
  else {
    if (s.cellRef(s.d_loc) < 5)
      s0 = 1;
    if (s.cellRef(s.e_loc) < 5)
      t0 = 1;
    if (s.cellRef(s.f_loc) < 5)
      u0 = 1;
  }

  // make 0 indexed
  const int ii = s.i - 1;
  const int jj = s.j - 1;

  return q[ii][jj][s.x][s0][t0][u0][static_cast<int>(a)];
}

void PDworld::setQUtil(PDstate &s, Qtable &q, Action a, double newUtil) {
  int s0 = 0, t0 = 0, u0 = 0;

  // not carrying
  if (s.x == 0) {
    if (s.cellRef(s.a_loc) >= 1)
      s0 = 1;
    if (s.cellRef(s.b_loc) >= 1)
      t0 = 1;
    if (s.cellRef(s.c_loc) >= 1)
      u0 = 1;
  }
  // carrying
  else {
    if (s.cellRef(s.d_loc) < 5)
      s0 = 1;
    if (s.cellRef(s.e_loc) < 5)
      t0 = 1;
    if (s.cellRef(s.f_loc) < 5)
      u0 = 1;
  }

  // make 0 indexed
  const int ii = s.i - 1;
  const int jj = s.j - 1;

  q[ii][jj][s.x][s0][t0][u0][static_cast<int>(a)] = newUtil;
}

double PDworld::getQLearningUtility(const double r, const double Qas,
                                    const double maxNextQas) {
  return ((1 - ALPHA) * Qas) + ALPHA * (r + (GAMMA * maxNextQas));
}

double PDworld::getSARSAUtility(const double r, const double Qas,
                                const double nextQas) {
  return Qas + (ALPHA * (r + (GAMMA * nextQas) - Qas));
}

// simple random number helper
int randomNumHelper(int lowInclusive, int highInclusive) {
  static std::mt19937 gen(std::random_device{}());

  std::uniform_int_distribution<> distr(lowInclusive, highInclusive);

  return distr(gen);
}

PDworld::Action PDworld::getOperationWithPRANDOM(std::vector<Action> &ops) {
  for (auto &o : ops) {
    if (o == Action::Dropoff || o == Action::Pickup) {
      return o;
    }
  }

  return ops[randomNumHelper(0, ops.size() - 1)];
}

PDworld::Action PDworld::getOperationWithPGREEDY(std::vector<Action> &ops,
                                                 PDstate &s, Qtable &q) {
  if (ops.empty()) {
    throw std::logic_error(
        "PDWORLD getOperationWithPGREEDY: ops was empty when received");
  }

  Action maxUtilOp;

  bool firstRunPast = false; // simple lock

  for (auto &o : ops) {
    if (o == Action::Dropoff || o == Action::Pickup) {
      return o;
    }

    // could be better but whatevs
    if (firstRunPast == false) {
      maxUtilOp = o;
      firstRunPast = true;
    } else if (getQUtil(s, q, o) == getQUtil(s, q, maxUtilOp)) {
      if (randomNumHelper(0, 1) == 1) {
        maxUtilOp = o;
      }
    } else if (getQUtil(s, q, o) > getQUtil(s, q, maxUtilOp)) {
      maxUtilOp = o;
    }
  }

  return maxUtilOp;
}

PDworld::Action PDworld::getOperationWithPEPLOIT(std::vector<Action> &ops,
                                                 PDstate &s, Qtable &q) {

  if (ops.empty()) {
    throw std::logic_error(
        "PDWORLD getOperationWithPGREEDY: ops was empty when received");
  }

  Action maxUtilOp;

  bool firstRunPast = false;

  for (auto &o : ops) {
    if (o == Action::Dropoff || o == Action::Pickup) {
      return o;
    }

    if (firstRunPast == false) {
      maxUtilOp = o;
      firstRunPast = true;
    } else if (getQUtil(s, q, o) == getQUtil(s, q, maxUtilOp)) {
      if (randomNumHelper(0, 1) == 1) {
        maxUtilOp = o;
      }
    } else if (getQUtil(s, q, o) > getQUtil(s, q, maxUtilOp)) {
      maxUtilOp = o;
    }
  }

  if (randomNumHelper(1, 10) <= 8) {
    return maxUtilOp;
  }

  std::vector<Action> others;
  for (auto &candidate : ops) {
    if (candidate != maxUtilOp) {
      others.push_back(candidate);
    }
  }

  if (!others.empty()) {
    return others[randomNumHelper(0, others.size() - 1)];
  }

  return maxUtilOp;
}

// will use PRANDOM, PGREEDY, PEPLOIT and DISPLAY
void PDworld::QLearning(std::vector<std::pair<int, std::string>> i,
                        const int experimentNum) {

  // initlization of PDworld
  PDstate worldState = initialState;
  Qtable Q{};

  int currSteps = 0;
  int newSteps = 0;
  std::string currPolicy = "N/A";

  for (auto &p : i) {
    // add newSteps to currSteps, and flush newSteps
    currSteps = newSteps;
    newSteps = 0;
    if (p.second == PRANDOM) {
      // PRANDOM Qlearning
      for (newSteps = 0; newSteps < currSteps + p.first; newSteps++) {
        currPolicy = PRANDOM;

        std::vector<Action> operations = aplop(worldState);
        Action op = getOperationWithPRANDOM(operations); // op to make
        int Qas = getQUtil(worldState, Q, op);           // current Qas
        PDstate prevWorldState = worldState; // capture prev world state
        double r = apply(worldState, op);    // change worldstate

        std::vector<Action> futureOps = aplop(worldState); // new worldstate
        Action maxNextOp = getOperationWithPGREEDY(
            futureOps, worldState, Q); // gets highest Q util next op
        double maxNextQas =
            getQUtil(worldState, Q, maxNextOp); // get that highest Q util value
        double calculatedUtil =
            getQLearningUtility(r, Qas, maxNextQas); // calculate util for Qas

        setQUtil(prevWorldState, Q, op, calculatedUtil); // set new Qas
      }
    } else if (p.second == PGREEDY) {
      // PGREEDY Qlearning
      for (newSteps = 0; newSteps < currSteps + p.first; newSteps++) {
        currPolicy = PGREEDY;

        std::vector<Action> operations = aplop(worldState);
        Action op =
            getOperationWithPGREEDY(operations, worldState, Q); // op to make
        int Qas = getQUtil(worldState, Q, op);                  // current Qas
        PDstate prevWorldState = worldState; // capture prev world state
        double r = apply(worldState, op);    // change worldstate

        std::vector<Action> futureOps = aplop(worldState); // new worldstate
        Action maxNextOp = getOperationWithPGREEDY(
            futureOps, worldState, Q); // gets highest Q util next op
        double maxNextQas =
            getQUtil(worldState, Q, maxNextOp); // get that highest Q util value
        double calculatedUtil =
            getQLearningUtility(r, Qas, maxNextQas); // calculate util for Qas

        setQUtil(prevWorldState, Q, op, calculatedUtil); // set new Qas
      }
    } else if (p.second == PEPLOIT) {
      // PEPLOIT Qlearning
      for (newSteps = 0; newSteps < currSteps + p.first; newSteps++) {
        currPolicy = PEPLOIT;

        std::vector<Action> operations = aplop(worldState);
        Action op =
            getOperationWithPEPLOIT(operations, worldState, Q); // op to make
        int Qas = getQUtil(worldState, Q, op);                  // current Qas
        PDstate prevWorldState = worldState; // capture prev world state
        double r = apply(worldState, op);    // change worldstate

        std::vector<Action> futureOps = aplop(worldState); // new worldstate
        Action maxNextOp = getOperationWithPGREEDY(
            futureOps, worldState, Q); // gets highest Q util next op
        double maxNextQas =
            getQUtil(worldState, Q, maxNextOp); // get that highest Q util value
        double calculatedUtil =
            getQLearningUtility(r, Qas, maxNextQas); // calculate util for Qas

        setQUtil(prevWorldState, Q, op, calculatedUtil); // set new Qas
      }
    } else if (p.second == DISPLAY) {
      // DISPLAY Qlearning
    }
  }
}

// will use PRANDOM, PEPLOIT and DISPLAY
void PDworld::SARSA(std::vector<std::pair<int, std::string>> i,
                    const int experimentNum) {

  // initlization of PDworld
  PDstate worldState = initialState;
  Qtable Q{};

  int currSteps = 0;
  int newSteps = 0;
  std::string currPolicy = "N/A";

  for (auto &p : i) {
    // add newSteps to currSteps, and flush newSteps
    currSteps = newSteps;
    newSteps = 0;
    if (p.second == PRANDOM) {
      // PRANDOM SARSA
      for (newSteps = 0; newSteps < currSteps + p.first; newSteps++) {
        currPolicy = PRANDOM;

        std::vector<Action> operations = aplop(worldState);
        Action op = getOperationWithPRANDOM(operations); // op to make
        int Qas = getQUtil(worldState, Q, op);           // current Qas
        PDstate prevWorldState = worldState; // capture prev world state
        double r = apply(worldState, op);    // change worldstate

        std::vector<Action> futureOps = aplop(worldState); // new worldstate
        Action randomNextOp =
            getOperationWithPRANDOM(operations); // re-run same policy
        double randomNextQas =
            getQUtil(worldState, Q, randomNextOp); // get QUtil
        double calculatedUtil =
            getSARSAUtility(r, Qas, randomNextQas); // calculate SARSA

        setQUtil(prevWorldState, Q, op, calculatedUtil); // set new Qas
      }
    } else if (p.second == PEPLOIT) {
      // PEPLOIT SARSA
      for (newSteps = 0; newSteps < currSteps + p.first; newSteps++) {
        currPolicy = PEPLOIT;

        std::vector<Action> operations = aplop(worldState);
        Action op =
            getOperationWithPEPLOIT(operations, worldState, Q); // op to make
        int Qas = getQUtil(worldState, Q, op);                  // current Qas
        PDstate prevWorldState = worldState; // capture prev world state
        double r = apply(worldState, op);    // change worldstate

        std::vector<Action> futureOps = aplop(worldState); // new worldstate
        Action randomNextOp = getOperationWithPEPLOIT(operations, worldState,
                                                      Q); // re-run same policy
        double randomNextQas =
            getQUtil(worldState, Q, randomNextOp); // get QUtil
        double calculatedUtil =
            getSARSAUtility(r, Qas, randomNextQas); // calculate SARSA

        setQUtil(prevWorldState, Q, op, calculatedUtil); // set new Qas
      }
    } else if (p.second == DISPLAY) {
      // DISPLAY SARSA
    }
  }
}
