#ifndef PDWORLDHELPERS_HPP
#define PDWORLDHELPERS_HPP

#include <iostream>

struct PDstate {
  int i{-1};   // xth position (-1 = *)
  int j{-1};   // yth position (-1 = *)
  bool x;      // true if carry else false
  int a, b, c; // pickup locations amount, (1,1) (3,3) (5,5)
  int d, e, f; // dropoff locations amount, (5,1) (5,3) (2,5)

  std::pair<int, int> a_loc, b_loc, c_loc; // pickup locations
  std::pair<int, int> d_loc, e_loc, f_loc; // dropoff locations
};

struct Rewards {
  int pickupReward;
  int dropoffReward;
  int nReward;
  int sReward;
  int eReward;
  int wReward;
};

#endif
