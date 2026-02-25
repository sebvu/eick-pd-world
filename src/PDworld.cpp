#include "PDworld.hpp"
#include <fstream>
#include <iomanip>
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

void PDworld::markdownDisplay(int expNum, std::string algName, int currSteps,
                              std::string currPolicy, PDstate &worldState,
                              Qtable &q) {

  auto actionSymbol = [&](Action a0) -> const char * {
    switch (a0) {
    case Action::North:
      return "^";
    case Action::South:
      return "v";
    case Action::East:
      return "->";
    case Action::West:
      return "<-";
    case Action::Pickup:
      return "P";
    case Action::Dropoff:
      return "D";
    default:
      return "?";
    }
  };

  auto stuboFor = [&](int xflag) {
    int s0 = 0, t0 = 0, u0 = 0;
    if (xflag == 0) {
      if (worldState.cellRef(worldState.a_loc) >= 1)
        s0 = 1;
      if (worldState.cellRef(worldState.b_loc) >= 1)
        t0 = 1;
      if (worldState.cellRef(worldState.c_loc) >= 1)
        u0 = 1;
    } else {
      if (worldState.cellRef(worldState.d_loc) < 5)
        s0 = 1;
      if (worldState.cellRef(worldState.e_loc) < 5)
        t0 = 1;
      if (worldState.cellRef(worldState.f_loc) < 5)
        u0 = 1;
    }
    return std::array<int, 3>{s0, t0, u0};
  };

  auto bestAt = [&](int i1, int j1, int xflag) {
    // i1/j1 are 1-indexed
    const int ii = i1 - 1;
    const int jj = j1 - 1;
    auto [s0, t0, u0] = stuboFor(xflag);

    // candidate actions (moves always exist in Qtable, but you may choose to
    // still display best direction; pickup/dropoff only make sense on special
    // cells)
    std::array<Action, 6> candidates = {Action::North,  Action::East,
                                        Action::South,  Action::West,
                                        Action::Pickup, Action::Dropoff};

    // filter applicability for pickup/dropoff based on current world counts
    auto applicable = [&](Action a0) {
      if (a0 == Action::Pickup) {
        if (xflag != 0)
          return false;
        if (std::pair<int, int>{i1, j1} != worldState.a_loc &&
            std::pair<int, int>{i1, j1} != worldState.b_loc &&
            std::pair<int, int>{i1, j1} != worldState.c_loc)
          return false;
        // at least one block in that pickup cell (based on current world)
        return worldState.cellRef({i1, j1}) >= 1;
      }
      if (a0 == Action::Dropoff) {
        if (xflag != 1)
          return false;
        if (std::pair<int, int>{i1, j1} != worldState.d_loc &&
            std::pair<int, int>{i1, j1} != worldState.e_loc &&
            std::pair<int, int>{i1, j1} != worldState.f_loc)
          return false;
        // dropoff cell has room
        return worldState.cellRef({i1, j1}) < 5;
      }
      return true; // moves always "allowed to compare"
    };

    Action bestA = Action::North;
    double bestQ = q[ii][jj][xflag][s0][t0][u0][static_cast<int>(bestA)];

    bool init = false;
    for (auto a0 : candidates) {
      if (!applicable(a0))
        continue;
      double val = q[ii][jj][xflag][s0][t0][u0][static_cast<int>(a0)];
      if (!init || val > bestQ) {
        bestQ = val;
        bestA = a0;
        init = true;
      } else if (val == bestQ) {
        // tiny tie-break randomness (optional): keep stable instead if you want
        if (randomNumHelper(0, 1) == 1)
          bestA = a0;
      }
    }
    return std::pair<double, Action>{bestQ, bestA};
  };

  std::ofstream md("pdworld_report.md", std::ios::app);
  if (!md)
    throw std::runtime_error("DISPLAY: failed to open pdworld_report.md");

  md << "\n---\n\n";
  md << "# Experiment: " << expNum << "\n";
  md << "- CurrStep: " << currSteps << "\n";
  md << "- Algorithm: " << algName << "\n";
  md << "- CurrentPolicy: " << currPolicy << "\n\n";

  auto writeTable = [&](int xflag) {
    md << "## Qtable (x=" << xflag << ")\n\n";
    // header row
    md << "|";
    for (int col = 1; col <= GRID_I; col++)
      md << " " << col << " |";
    md << "\n|";
    for (int col = 1; col <= GRID_I; col++)
      md << " --- |";
    md << "\n";

    // rows j=1..5 (top to bottom). If you want (1,1) at bottom-left, reverse
    // this.
    for (int row = 1; row <= GRID_J; row++) {
      md << "|";
      for (int col = 1; col <= GRID_I; col++) {
        auto [qv, ba] = bestAt(col, row, xflag);
        md << " " << std::fixed << std::setprecision(2) << qv << " "
           << actionSymbol(ba) << " |";
      }
      md << "\n";
    }
    md << "\n";
  };

  writeTable(0);
  writeTable(1);
}

// will use PRANDOM, PGREEDY, PEPLOIT and DISPLAY
void PDworld::QLearning(std::vector<std::pair<int, std::string>> i,
                        const int experimentNum) {

  // initlization of PDworld
  PDstate worldState = initialState;
  Qtable Q{};

  std::string algName = "QLearning";
  int currSteps = 0;
  std::string currPolicy = "N/A";

  for (auto &p : i) {
    if (p.second == PRANDOM) {
      // PRANDOM Qlearning
      for (int k = 0; k < p.first; k++) {
        currSteps++;
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
      for (int k = 0; k < p.first; k++) {
        currSteps++;
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
      for (int k = 0; k < p.first; k++) {
        currSteps++;
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
      markdownDisplay(experimentNum, algName, currSteps, currPolicy, worldState,
                      Q);
    }
  }
}

// will use PRANDOM, PEPLOIT and DISPLAY
void PDworld::SARSA(std::vector<std::pair<int, std::string>> i,
                    const int experimentNum) {

  // initlization of PDworld
  PDstate worldState = initialState;
  Qtable Q{};

  std::string algName = "SARSA";
  int currSteps = 0;
  std::string currPolicy = "N/A";

  for (auto &p : i) {
    if (p.second == PRANDOM) {
      // PRANDOM SARSA
      for (int k = 0; k < p.first; k++) {
        currSteps++;
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
      for (int k = 0; k < p.first; k++) {
        currSteps++;
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
      markdownDisplay(experimentNum, algName, currSteps, currPolicy, worldState,
                      Q);
    }
  }
}
