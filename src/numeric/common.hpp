/**
 * @file common.hpp
 * @brief This header contains the definitions of base types for numbers used by
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

/// Real number type
using real = double;
/// Complex number type
using complex = std::complex<real>;

/// Shorthand for writing complex numbers in C++
complex operator""_i(unsigned long long val);

/// Shorthand for writing complex numbers in C++
complex operator""_i(long double val);
/// @}

/// Throws an exception if val is not in the range [min, max_m1)
inline void verify_in_bounds(int val, int min, int max_m1) {
  if (val < min || val >= max_m1) {
    throw std::out_of_range("Out of range access");
  }
}

/// Matrix type-parametrized vector constructor
template <class M> inline M make_vector(std::initializer_list<complex> cdata);
/// Matrix type-parametrized covector constructor
template <class M> inline M make_covector(std::initializer_list<complex> cdata);

/// Kronecker product for a given span of matrices
template <class M, ptrdiff_t N>
inline M kronecker(gsl::span<const M *, N> mats);

/// Kronecker product
/// Usage: kronecker({mat1, mat2, mat3});
template <class M> inline M kronecker(std::initializer_list<const M *> mats) {
  return kronecker<M, gsl::dynamic_extent>(
      gsl::make_span(const_cast<const M **>(mats.begin()),
                     const_cast<const M **>(mats.end())));
}

} // namespace qc
