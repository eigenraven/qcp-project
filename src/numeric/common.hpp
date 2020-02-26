/**
 * @file This header contains the definitions of base types for numbers used by
 * the simulator, and includes used supporting libraries
 */
#pragma once

#include <algorithm>
#include <cassert>
#include <climits>
#include <cmath>
#include <complex>
#include <exception>
#include <functional>
#include <gsl/gsl>
#include <numeric>
#include <optional>
#include <vector>

namespace qc {

/** @{
 * The basic data types for numbers used in the simulator
 */
using real = double;
using complex = std::complex<real>;

complex operator""_i(long double val);
/// @}

inline void verify_in_bounds(int val, int min, int max_m1) {
  if (val < min || val >= max_m1) {
    throw std::out_of_range("Out of range access");
  }
}

} // namespace qc
