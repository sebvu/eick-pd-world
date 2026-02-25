#ifndef PDWORLDHELPERS_HPP
#define PDWORLDHELPERS_HPP

#include <map>
#include <stdexcept>

struct PDstate {
  int i{-1};   // xth position (-1 = *)
  int j{-1};   // yth position (-1 = *)
  int x{0};    // true if carry else false
  int a, b, c; // pickup locations amount, (1,1) (3,3) (5,5)
  int d, e, f; // dropoff locations amount, (5,1) (5,3) (2,5)

  const std::pair<int, int> a_loc, b_loc, c_loc; // pickup locations
  const std::pair<int, int> d_loc, e_loc, f_loc; // dropoff locations

  // accessor to a, b, c, d, e, f

  int &cellRef(const std::pair<int, int> &p) {
    if (p == a_loc)
      return a;
    if (p == b_loc)
      return b;
    if (p == c_loc)
      return c;
    if (p == d_loc)
      return d;
    if (p == e_loc)
      return e;
    if (p == f_loc)
      return f;
    throw std::logic_error("PDstate::cellRef: p is not a special cell");
  }

  const int &cellRef(const std::pair<int, int> &p)
      const { // overload to handle diff const and non-const refs
    if (p == a_loc)
      return a;
    if (p == b_loc)
      return b;
    if (p == c_loc)
      return c;
    if (p == d_loc)
      return d;
    if (p == e_loc)
      return e;
    if (p == f_loc)
      return f;
    throw std::logic_error("PDstate::cellRef: p is not a special cell");
  }
};

struct Rewards {
  const int pickupReward;
  const int dropoffReward;
  const int nReward;
  const int sReward;
  const int eReward;
  const int wReward;
};

#endif
