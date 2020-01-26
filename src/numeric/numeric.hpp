/**
 * @file This header contains the definitions of all numeric primitives used by
 * the simulator, like vectors, matrices; and operations on them.
 */
#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <complex>
#include <exception>
#include <vector>
#include <climits>

using namespace std::complex_literals;

namespace qc {

/** @{
 * The basic data types for numbers used in the simulator
 */
using real = double;
using complex = std::complex<real>;
/// @}

inline void verify_in_bounds(int val, int min, int max_m1) {
  if (val < min || val >= max_m1) {
    throw std::out_of_range("Out of range access");
  }
}

/// A dense complex matrix type (stores fields in a contiguous row-major array)
struct dmatrix {
  /// Dimensions of the matrix
  int rows, cols;
  /// Row-major data array
  std::vector<complex> data;

  /// Default constructs a zero-filled matrix
  inline explicit dmatrix(int rows, int cols)
      : rows(rows), cols(cols), data(rows * cols) {
    verify_in_bounds(rows, 0, INT_MAX);
    verify_in_bounds(cols, 0, INT_MAX);
  }
  /// Constructs a matrix with given elements (given in a row-major order)
  template <int N>
  inline explicit dmatrix(int rows, int cols, const complex (&cdata)[N])
      : rows(rows), cols(cols), data(rows * cols) {
    if (rows * cols != N) {
      throw std::invalid_argument("dmatrix rows*cols != length(data)");
    }
    verify_in_bounds(rows, 0, INT_MAX);
    verify_in_bounds(cols, 0, INT_MAX);
    std::copy_n(cdata, N, data.begin());
  }

  /// Creates a new zero matrix
  static inline dmatrix zero(int rows, int cols) { return dmatrix(rows, cols); }
  /// Creates an identity matrix with 1's on the diagonal
  static inline dmatrix identity(int rows, int cols) {
    dmatrix m{rows, cols};
    for (int i = 0; i < rows && i < cols; i++) {
      m(i, i) = 1.0;
    }
    return m;
  }
  /// Creates a column vector matrix
  template <int Dim> static inline dmatrix vector(const complex (&cdata)[Dim]) {
    return dmatrix(Dim, 1, cdata);
  }

  /// Calculates the position of a given element in the data array
  inline size_t element_offset(int row, int col) const {
    return col + cols * row;
  }

  /// Provides access to the matrix elements
  inline complex &operator()(int row, int col) {
    verify_in_bounds(row, 0, rows);
    verify_in_bounds(col, 0, cols);
    return data[element_offset(row, col)];
  }
  inline const complex &operator()(int row, int col) const {
    verify_in_bounds(row, 0, rows);
    verify_in_bounds(col, 0, cols);
    return data[element_offset(row, col)];
  }

  /// Creates the transpose of the matrix
  inline dmatrix T() const {
    dmatrix m{cols, rows};
    for (int row = 0; row < rows; row++) {
      for (int col = 0; col < cols; col++) {
        m(col, row) = (*this)(row, col);
      }
    }
    return m;
  }
  /// Creates the hermitian transpose of the matrix
  inline dmatrix H() const {
    dmatrix m{cols, rows};
    for (int row = 0; row < rows; row++) {
      for (int col = 0; col < cols; col++) {
        m(col, row) = std::conj((*this)(row, col));
      }
    }
    return m;
  }
};

/// Type alias for vectors for clarity
using dvector = dmatrix;

} // namespace qc
