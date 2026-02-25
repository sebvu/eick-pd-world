#ifndef PDWORLDHELPERS_HPP
#define PDWORLDHELPERS_HPP

#include <map>

struct PDstate {
  int i{-1};     // xth position (-1 = *)
  int j{-1};     // yth position (-1 = *)
  bool x{false}; // true if carry else false
  int a, b, c;   // pickup locations amount, (1,1) (3,3) (5,5)
  int d, e, f;   // dropoff locations amount, (5,1) (5,3) (2,5)

  const std::pair<int, int> a_loc, b_loc, c_loc; // pickup locations
  const std::pair<int, int> d_loc, e_loc, f_loc; // dropoff locations

  // accessor to a, b, c, d, e, f
  std::map<const std::pair<int, int>, int *> loc_val = {
      {a_loc, &a}, {b_loc, &b}, {c_loc, &c},
      {d_loc, &d}, {e_loc, &e}, {f_loc, &f}};
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
