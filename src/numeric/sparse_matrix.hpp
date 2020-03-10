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

/// A reference used for reading/modifying the sparse matrix by element position
struct sparse_entry_ref {
  std::vector<sparse_entry> &row;
  int column;
  int index;

  inline std::optional<sparse_entry *> find_entry() {
    if (index >= row.size()) {
      return std::nullopt;
    }
    sparse_entry &en = row.at(index);
    if (en.column != this->column) {
      return std::nullopt;
    } else {
      return &en;
    }
  }

  inline complex value() {
    auto en = find_entry();
    if (en.has_value()) {
      return en.value()->value;
    } else {
      return 0.0;
    }
  }

  inline void set_value(complex newVal) {
    auto en = find_entry();
    if (en.has_value()) {
      if (newVal == 0.0) {
        // remove existing entry
        row.erase(row.begin() + this->index);
      } else {
        // update value
        en.value()->value = newVal;
      }
    } else {
      if (newVal == 0.0) {
        // no-op
      } else {
        // insert value
        row.insert(row.begin() + this->index, sparse_entry{this->column, newVal});
      }
    }
  }

  inline sparse_entry_ref &operator=(complex newVal) {
    this->set_value(newVal);
    return *this;
  }
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

  inline std::vector<complex> to_std_vector() const {
    std::vector<complex> v;
    v.resize(this->rows * this->cols, 0.0 + 0.0_i);
    for (int row = 0; row < rows; row++) {
      const auto &data = this->row_data.at(row);
      for (const auto &el : data) {
        v.at(row * this->cols + el.column) = el.value;
      }
    }
    return v;
  }

  inline sparse_entry_ref find_ref(int row, int col) {
    auto &vrow = this->row_data.at(row);
    return sparse_entry_ref{vrow, col,
                            int(std::lower_bound(vrow.begin(), vrow.end(), col,
                                             [](const sparse_entry &en, int cidx) {
                                               return en.column < cidx;
                                             }) - vrow.begin())};
  }

  inline sparse_entry_ref operator()(int row, int col) {
    return find_ref(row, col);
  }

  inline smatrix T() const { return zero(1,1); }

  inline smatrix H() const { return zero(1,1); }

  inline bool is_vector() const { return cols == 1; }

  inline bool is_covector() const { return rows == 1; }
};

inline bool operator==(const sparse_entry &a, const sparse_entry &b) {
  return a.column == b.column && a.value == b.value;
}

inline bool operator==(const smatrix &a, const smatrix &b) {
  return a.rows == b.rows && a.cols == b.cols && a.row_data == b.row_data;
}

inline bool operator!=(const smatrix &a, const smatrix &b) { return !(a == b); }

inline smatrix operator+(const smatrix& a, const smatrix& b) {
  if (a.cols != b.cols || a.rows != b.rows) {
    throw std::invalid_argument("Trying to add matrices of different sizes");
  }
  smatrix r{a.rows, a.cols};
  return r;
}

inline smatrix operator-(const smatrix &a, const smatrix &b) {
  if (a.cols != b.cols || a.rows != b.rows) {
    throw std::invalid_argument("Trying to subtract matrices of different sizes");
  }
  smatrix r{a.rows, a.cols};
  return r;
}

inline smatrix operator-(const smatrix &a) {
  smatrix r{a.rows, a.cols};
  return r;
}

inline smatrix operator*(const smatrix &a, complex b) {
  smatrix r{a.rows, a.cols};
  return r;
}

inline smatrix operator*(const smatrix &a, real b) { return a * complex(b); }

inline smatrix operator*(complex a, const smatrix &b) { return b * a; }

inline smatrix operator*(real a, const smatrix &b) { return b * complex(a); }

/// Type alias for vectors for clarity
using svector = smatrix;

/// Shorthand to make column vector matrices
inline smatrix make_svector(std::initializer_list<complex> cdata) {
  return smatrix{static_cast<int>(cdata.size()), 1, cdata};
}

/// Shorthand to make row vector matrices
inline smatrix make_scovector(std::initializer_list<complex> cdata) {
  return smatrix{1, static_cast<int>(cdata.size()), cdata};
}

template <>
inline smatrix make_vector<smatrix>(std::initializer_list<complex> cdata) {
  return make_svector(cdata);
}

template <>
inline smatrix make_covector<smatrix>(std::initializer_list<complex> cdata) {
  return make_scovector(cdata);
}

/// Matrix-matrix multiplication
inline smatrix operator*(const smatrix& a, const smatrix& b) {
  if (a.cols != b.rows) {
    throw std::invalid_argument(
        "Mismatched dimensions for matrix multiplication");
  }
  smatrix r{a.rows, b.cols};
  return r;
}

/// Dot product of two vectors (Nx1 matrices) = a^H * b
inline complex dot(const svector &a, const svector &b) {
  if (!a.is_vector() || !b.is_vector()) {
    throw std::invalid_argument(
        "Trying to calculate dot product of non-vector matrices");
  }
  if (a.rows != b.rows) {
    throw std::invalid_argument(
        "Trying to calculate dot product of vectors of different dimensions");
  }
  auto adata = a.to_std_vector();
  auto bdata = b.to_std_vector();
  return std::inner_product(
      adata.cbegin(), adata.cend(), bdata.cbegin(), complex{0, 0},
      std::plus<>(),
      [](const complex &a, const complex &b) { return std::conj(a) * b; });
}

/// Outer product of two vectors (Nx1 matrices) = a * b^H
inline smatrix outer(const svector &a, const svector &b) {
  if (!a.is_vector() || !b.is_vector()) {
    throw std::invalid_argument(
        "Trying to calculate outer product of non-vector matrices");
  }
  smatrix r{a.rows, b.rows};
  return r;
}

template <>
inline smatrix kronecker<smatrix, gsl::dynamic_extent>(gsl::span<const smatrix *, gsl::dynamic_extent> mats) {
  std::vector<int> row_idx(mats.size()), col_idx(mats.size());
  int total_rows =
      std::accumulate(mats.cbegin(), mats.cend(), 1,
                      [](int acc, const smatrix *m) { return acc * m->rows; });
  int total_cols =
      std::accumulate(mats.cbegin(), mats.cend(), 1,
                      [](int acc, const smatrix *m) { return acc * m->cols; });
  smatrix kp{total_rows, total_cols};
  return kp;
}


} // namespace qc
