#ifndef PDWORLD_HPP
#define PDWORLD_HPP

#include <set>
#include <string>

/*
  For both algorithms, these are the constant details

  α = 0.3 (learning rate)
  γ = 0.5 (discount rate)

  (i, j, x, a, b, c, d, e, f) with
  - (i,j) is the position of the agent
  - x is 1 if the agent carries a block and 0 if not
  - (a,b,c,d,e,f) are the number of blocks in cells
      (1,1), (3,3),(5,5), (5,1), (5,3), and (2,5), respectively
        !! ASSUMING 1 INDEXED !!

  agent starts at (1,5)

  initial state:(1,5,0,5,5,5,0,0,0)
  terminal state: (*,*,0,0,0,0,5,5,5)

  pickup block from pickup state: +13
  drop off a block at dropoff state: +13
  applying n,s,e,w: -1

  PEXPLOIT
  PGREEDY
  PRANDOM

*/

class PDworld {
private:
  class StateWorld {
  private:
    int i, j, a, b, c, d, e, f;
    int ti, tj, ta, tb, tc, td, te, tf;
    bool x, tx;

  public:
    StateWorld(int i, int j, bool x, int a, int b, int c, int d, int e, int f,
               int ti, int tj, int tx, int ta, int tb, int tc, int td, int te,
               int tf)
        : i(i), j(j), a(a), b(b), c(c), d(d), e(e), f(f), x(x) {}

    bool TerminalStateReached() {
      return (i == ti && j == tj && a == ta && b == tb && c == tc && d == td &&
              e == te && f == tf && x == tx)
                 ? true
                 : false;
    };
  };

  StateWorld state;
  int gridX, gridY;
  static constexpr std::string_view PGREEDY = "PGREEDY", PEXPLOIT = "PEXPLOIT",
                                    PRANDOM = "PRANDOM";

public:
  PDworld(int i, int j, bool x, int a, int b, int c, int d, int e, int f,
          int gridX, int gridY, int ti, int tj, bool tx, int ta, int tb, int tc,
          int td, int te, int tf);
};

#endif
