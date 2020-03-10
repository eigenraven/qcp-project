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

complex operator""_i(unsigned long long val);
complex operator""_i(long double val);
/// @}

inline void verify_in_bounds(int val, int min, int max_m1) {
  if (val < min || val >= max_m1) {
    throw std::out_of_range("Out of range access");
  }
}

template <class M> inline M make_vector(std::initializer_list<complex> cdata);
template <class M> inline M make_covector(std::initializer_list<complex> cdata);

/// Kronecker product for a given span of matrices
template <class M, ptrdiff_t N> inline M kronecker(gsl::span<const M *, N> mats);

/// Kronecker product
/// Usage: kronecker({mat1, mat2, mat3});
template <class M> inline M kronecker(std::initializer_list<const M *> mats) {
  return kronecker(gsl::make_span(mats.begin(), mats.end()));
}

} // namespace qc
