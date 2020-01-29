/**
 * @file This header contains the definitions of all numeric primitives used by
 * the simulator, like vectors, matrices; and operations on them.
 */
#pragma once

#include <algorithm>
#include <cassert>
#include <climits>
#include <cmath>
#include <complex>
#include <exception>
#include <optional>
#include <vector>

namespace qc {

/** @{
 * The basic data types for numbers used in the simulator
 */
using real = double;
using complex = std::complex<real>;

complex operator""_i(long double val) {
  return complex{0, static_cast<real>(val)};
}
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
  template <int N>
  inline explicit dmatrix(int rows, int cols, const real (&cdata)[N])
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

  inline bool is_vector() const { return cols == 1; }

  inline bool is_covector() const { return rows == 1; }

  complex determinant() const;

  std::optional<dmatrix> inverse() const;

  /// Checks if this matrix is unitary
  /// Warning: slow
  bool is_unitary() const;
};

inline bool operator==(const dmatrix &a, const dmatrix &b) {
  return a.rows == b.rows && a.cols == b.cols && a.data == b.data;
}

inline bool operator!=(const dmatrix &a, const dmatrix &b) { return !(a == b); }

inline dmatrix operator+(const dmatrix &a, const dmatrix &b) {
  if (a.cols != b.cols || a.rows != b.rows) {
    throw std::invalid_argument("Trying to add matrices of different sizes");
  }
  dmatrix r{a.rows, a.cols};
  std::transform(a.data.cbegin(), a.data.cend(), b.data.cbegin(),
                 r.data.begin(), [](complex x, complex y) { return x + y; });
  return r;
}

inline dmatrix operator-(const dmatrix &a, const dmatrix &b) {
  if (a.cols != b.cols || a.rows != b.rows) {
    throw std::invalid_argument(
        "Trying to subtract matrices of different sizes");
  }
  dmatrix r{a.rows, a.cols};
  std::transform(a.data.cbegin(), a.data.cend(), b.data.cbegin(),
                 r.data.begin(), [](complex x, complex y) { return x - y; });
  return r;
}

inline dmatrix operator-(const dmatrix &a) {
  dmatrix r{a.rows, a.cols};
  std::transform(a.data.cbegin(), a.data.cend(), r.data.begin(),
                 [](complex x) { return -x; });
  return r;
}

inline dmatrix operator*(const dmatrix &a, complex b) {
  dmatrix r{a.rows, a.cols};
  std::transform(a.data.cbegin(), a.data.cend(), r.data.begin(),
                 [b](complex x) { return x * b; });
  return r;
}

inline dmatrix operator*(const dmatrix &a, real b) { return a * complex(b); }

inline dmatrix operator*(complex a, const dmatrix &b) { return b * a; }

inline dmatrix operator*(real a, const dmatrix &b) { return b * complex(a); }

/// Type alias for vectors for clarity
using dvector = dmatrix;

/// Matrix-matrix multiplication
inline dmatrix operator*(const dmatrix &a, const dmatrix &b) { assert(0); }

/// Dot product of two vectors (Nx1 matrices) = a^H * b
inline complex dot(const dvector &a, const dvector &b) { assert(0); }

/// Outer product of two vectors (Nx1 matrices) = a * b^H
inline dmatrix outer(const dvector &a, const dvector &b) { assert(0); }

inline dmatrix kronecker_dense(const dmatrix matrices[],
                               size_t matrices_count) {
  assert(0);
}

/// Kronecker product producing a dense matrix
/// Usage: kronecker_dense({mat1, mat2, mat3});
template <size_t N> inline dmatrix kronecker_dense(const dmatrix (&mats)[N]) {
  return kronecker_dense(mats, N);
}

} // namespace qc
