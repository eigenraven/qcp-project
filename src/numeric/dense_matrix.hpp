/**
 * @file Dense (contiguous array) matrix representation implementation.
 */
#pragma once
#include "common.hpp"

namespace qc {

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
  inline explicit dmatrix(int rows, int cols,
                          std::initializer_list<complex> cdata)
      : rows(rows), cols(cols), data(cdata) {
    if (rows * cols != cdata.size()) {
      throw std::invalid_argument("dmatrix rows*cols != length(data)");
    }
    verify_in_bounds(rows, 0, INT_MAX);
    verify_in_bounds(cols, 0, INT_MAX);
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

  inline complex &unchecked_at(int row, int col) {
    return data[element_offset(row, col)];
  }
  inline const complex &unchecked_at(int row, int col) const {
    return data[element_offset(row, col)];
  }

  inline std::vector<complex> to_std_vector() const { return this->data; }

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

  /// Create a matrix clone of this with a removed row
  inline dmatrix removed_row(int deleted_row) const {
    verify_in_bounds(deleted_row, 0, rows);
    dmatrix r{rows - 1, cols};
    for (int row = 0; row < rows - 1; row++) {
      for (int col = 0; col < cols; col++) {
        r.unchecked_at(row, col) =
            this->unchecked_at(row >= deleted_row ? row + 1 : row, col);
      }
    }
    return r;
  }

  /// Create a matrix clone of this with a removed column
  inline dmatrix removed_column(int deleted_col) const {
    verify_in_bounds(deleted_col, 0, cols);
    dmatrix r{rows, cols - 1};
    for (int row = 0; row < rows; row++) {
      for (int col = 0; col < cols - 1; col++) {
        r.unchecked_at(row, col) =
            this->unchecked_at(row, col >= deleted_col ? col + 1 : col);
      }
    }
    return r;
  }

  /// Create a matrix clone of this with a removed row and column
  inline dmatrix removed_row_and_column(int deleted_row,
                                        int deleted_col) const {
    verify_in_bounds(deleted_row, 0, rows);
    verify_in_bounds(deleted_col, 0, cols);
    dmatrix r{rows - 1, cols - 1};
    for (int row = 0; row < rows - 1; row++) {
      for (int col = 0; col < cols - 1; col++) {
        r.unchecked_at(row, col) =
            this->unchecked_at(row >= deleted_row ? row + 1 : row,
                               col >= deleted_col ? col + 1 : col);
      }
    }
    return r;
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

/// Shorthand to make column vector matrices
inline dmatrix make_dvector(std::initializer_list<complex> cdata) {
  return dmatrix{static_cast<int>(cdata.size()), 1, cdata};
}

/// Shorthand to make row vector matrices
inline dmatrix make_dcovector(std::initializer_list<complex> cdata) {
  return dmatrix{1, static_cast<int>(cdata.size()), cdata};
}

template <>
inline dmatrix make_vector<dmatrix>(std::initializer_list<complex> cdata) {
  return make_dvector(cdata);
}

template <>
inline dmatrix make_covector<dmatrix>(std::initializer_list<complex> cdata) {
  return make_dcovector(cdata);
}

/// Matrix-matrix multiplication
inline dmatrix operator*(const dmatrix &a, const dmatrix &b) {
  if (a.cols != b.rows) {
    throw std::invalid_argument(
        "Mismatched dimensions for matrix multiplication");
  }
  dmatrix r{a.rows, b.cols};
  for (int row = 0; row < r.rows; row++) {
    for (int col = 0; col < r.cols; col++) {
      for (int elem = 0; elem < a.cols; elem++) {
        r.unchecked_at(row, col) +=
            a.unchecked_at(row, elem) * b.unchecked_at(elem, col);
      }
    }
  }
  return r;
}

/// Dot product of two vectors (Nx1 matrices) = a^H * b
inline complex dot(const dvector &a, const dvector &b) {
  if (!a.is_vector() || !b.is_vector()) {
    throw std::invalid_argument(
        "Trying to calculate dot product of non-vector matrices");
  }
  if (a.rows != b.rows) {
    throw std::invalid_argument(
        "Trying to calculate dot product of vectors of different dimensions");
  }
  return std::inner_product(
      a.data.cbegin(), a.data.cend(), b.data.cbegin(), complex{0, 0},
      std::plus<>(),
      [](const complex &a, const complex &b) { return std::conj(a) * b; });
}

/// Outer product of two vectors (Nx1 matrices) = a * b^H
inline dmatrix outer(const dvector &a, const dvector &b) {
  if (!a.is_vector() || !b.is_vector()) {
    throw std::invalid_argument(
        "Trying to calculate outer product of non-vector matrices");
  }
  dmatrix r{a.rows, b.rows};
  auto it = r.data.begin();
  for (int row = 0; row < a.rows; row++, it += a.rows) {
    complex e = a.data[row];
    std::transform(b.data.cbegin(), b.data.cend(), it,
                   [e](complex v) { return e * v; });
  }
  return r;
}

template <ptrdiff_t N>
inline dmatrix kronecker_dense(
    gsl::span<const std::reference_wrapper<const dmatrix>, N> mats) {
  std::vector<int> row_idx(mats.size()), col_idx(mats.size());
  int total_rows =
      std::accumulate(mats.cbegin(), mats.cend(), 1,
                      [](int acc, const dmatrix &m) { return acc * m.rows; });
  int total_cols =
      std::accumulate(mats.cbegin(), mats.cend(), 1,
                      [](int acc, const dmatrix &m) { return acc * m.cols; });
  dmatrix kp{total_rows, total_cols};
  for (int r = 0; r < total_rows; r++) {
    for (int c = 0; c < total_cols; c++) {
      // calculate product for the element
      complex P = 1.0;
      for (int mi = 0; P != 0.0 && mi < mats.size(); mi++) {
        P *= mats[mi](row_idx[mi], col_idx[mi]);
      }
      kp(r, c) = P;
      // increase column index
      for (int p = col_idx.size() - 1; p >= 0; p--) {
        col_idx[p]++;
        if (col_idx[p] >= mats[p].get().cols) {
          col_idx[p] = 0;
        } else {
          break;
        }
      }
    }
    // reset row indices before next iteration
    std::fill(col_idx.begin(), col_idx.end(), 0);
    // increase row index
    for (int p = row_idx.size() - 1; p >= 0; p--) {
      row_idx[p]++;
      if (row_idx[p] >= mats[p].get().rows) {
        row_idx[p] = 0;
      } else {
        break;
      }
    }
  }
  return kp;
}

/// Kronecker product producing a dense matrix
/// Usage: kronecker_dense({mat1, mat2, mat3});
inline dmatrix kronecker_dense(
    std::initializer_list<std::reference_wrapper<const dmatrix>> mats) {
  return kronecker_dense(gsl::make_span(mats.begin(), mats.end()));
}

} // namespace qc
