#ifndef PDWORLD_HPP
#define PDWORLD_HPP

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

*/

class PDWorld {
private:
  class StateWorld {
  private:
    int i, j, a, b, c, d, e, f;
    bool x;

  public:
    StateWorld(int i, int j, bool x, int a, int b, int c, int d, int e, int f)
        : i(i), j(j), a(a), b(b), c(c), d(d), e(e), f(f), x(x) {}
  };

  StateWorld state;

public:
  PDWorld(int i, int j, bool x, int a, int b, int c, int d, int e, int f)
      : state(StateWorld(i, j, x, a, b, c, d, e, f)) {}
};

#endif
