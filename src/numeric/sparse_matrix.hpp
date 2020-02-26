/**
 * @file Sparse (array of rows, rows being arrays of row elements) matrix
 * representation implementation.
 */
#pragma once
#include "common.hpp"

namespace qc {

/// A single non-zero entry in a sparse matrix
struct sparse_entry {
  int column;
  complex value;
};

/// A dense complex matrix type (stores fields in a contiguous row-major array)
struct smatrix {
  /// Dimensions of the matrix
  int rows, cols;
  /// Data array-of-arrays
  std::vector<std::vector<sparse_entry>> row_data;

  /// Default constructs a zero-filled matrix
  inline explicit smatrix(int rows, int cols)
      : rows(rows), cols(cols), row_data(rows) {
    verify_in_bounds(rows, 0, INT_MAX);
    verify_in_bounds(cols, 0, INT_MAX);
  }
  /// Constructs a matrix with given elements (given in a row-major order)
  inline explicit smatrix(int rows, int cols,
                          std::initializer_list<complex> cdata)
      : rows(rows), cols(cols), row_data(rows) {
    if (rows * cols != cdata.size()) {
      throw std::invalid_argument("smatrix rows*cols != length(data)");
    }
    verify_in_bounds(rows, 0, INT_MAX);
    verify_in_bounds(cols, 0, INT_MAX);
    int el = 0;
    for (int row = 0; row < rows; row++) {
      for (int col = 0; col < cols; col++) {
        complex value = cdata.begin()[el];
        if (value != 0.0) {
          this->row_data[row].push_back({col, value});
        }
        el++;
      }
    }
  }

  /// Creates a new zero matrix
  static inline smatrix zero(int rows, int cols) { return smatrix(rows, cols); }
  /// Creates an identity matrix with 1's on the diagonal
  static inline smatrix identity(int rows, int cols) {
    smatrix m{rows, cols};
    for (int i = 0; i < rows && i < cols; i++) {
      m.row_data[i].push_back({i, complex(1.0)});
    }
    return m;
  }
  /// Creates a column vector matrix
  template <int Dim> static inline smatrix vector(const complex (&cdata)[Dim]) {
    return smatrix(Dim, 1, cdata);
  }
};

} // namespace qc
