/**
 * @file sparse_matrix.hpp
 * @brief Sparse (array of rows, rows being arrays of row elements) matrix
 * representation implementation.
 */
#pragma once
#include "common.hpp"
#include <vector>

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

  /// Finds the entry if it's present in the matrix (non-zero)
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

  /// Accesses the value of the entry, or zero if it's not present
  inline complex value() {
    auto en = find_entry();
    if (en.has_value()) {
      return en.value()->value;
    } else {
      return 0.0;
    }
  }

  /// Updates the sparse matrix with the appropriate value
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
        row.insert(row.begin() + this->index,
                   sparse_entry{this->column, newVal});
      }
    }
  }

  /// set_value() shorthand
  inline sparse_entry_ref &operator=(complex newVal) {
    this->set_value(newVal);
    return *this;
  }

  /// value() shorthand
  inline operator complex() { return this->value(); }
};

/// Element of a sparse matrix as returned by an iterator
struct sparse_nonzero_element {
  int row;
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
      : smatrix(rows, cols,
                gsl::make_span(const_cast<complex *>(cdata.begin()),
                               const_cast<complex *>(cdata.end()))) {}
  /// Constructs a matrix with given elements (given in a row-major order)
  inline explicit smatrix(int rows, int cols,
                          gsl::span<complex, gsl::dynamic_extent> cdata)
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

  /// Converts the matrix to a vector of elements in row-major order (creates a
  /// copy of the data)
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

  /// Finds an element position in the sparse array
  inline sparse_entry_ref find_ref(int row, int col) {
    auto &vrow = this->row_data.at(row);
    return sparse_entry_ref{
        vrow, col,
        int(std::lower_bound(vrow.begin(), vrow.end(), col,
                             [](const sparse_entry &en, int cidx) {
                               return en.column < cidx;
                             }) -
            vrow.begin())};
  }

  /// Accesses an element in the sparse matrix (read and write through the
  /// sparse_entry_ref wrapper object)
  inline sparse_entry_ref operator()(int row, int col) {
    return find_ref(row, col);
  }

  /// Creates a transpose of the matrix
  inline smatrix T() const {
    smatrix r{this->cols, this->rows};
    if (this->rows > 0) {
      for (int row = 0; row < r.rows; row++) {
        r.row_data.at(row).reserve(this->row_data[0].size());
      }
    }
    for (int row = 0; row < this->rows; row++) {
      for (auto &el : this->row_data.at(row)) {
        r.row_data.at(el.column).push_back(sparse_entry{row, el.value});
      }
    }
    r.fix_column_order();
    return r;
  }

  /// Sorts the elements by column id in case of manual construction of the data array
  inline void fix_column_order() {
    for (int row = 0; row < this->rows; row++) {
      auto &rdata = this->row_data.at(row);
      std::sort(rdata.begin(), rdata.end(),
                [](const sparse_entry &a, const sparse_entry &b) {
                  return a.column < b.column;
                });
    }
  }

  /// Creates a hermitian transpose of the matrix
  inline smatrix H() const {
    smatrix r{this->T()};
    for (auto &row : r.row_data) {
      for (auto &el : row) {
        el.value = std::conj(el.value);
      }
    }
    return r;
  }

  /// Checks if this is a column vector
  inline bool is_vector() const { return cols == 1; }

  /// Checks if this is a row vector
  inline bool is_covector() const { return rows == 1; }

  /// Returns the number of non-zero elements in this matrix
  inline int64_t count_nonzero() const {
    int64_t count{0};
    for (const auto &row : this->row_data) {
      count += row.size();
    }
    return count;
  }
};

/// Utility iterator over all non-zero elements of the matrix, use like any C++ standard iterator
struct sparse_iterator {
  typedef sparse_nonzero_element &reference;
  typedef sparse_nonzero_element value_type;

  bool end{true};
  const smatrix *matrix;
  int row;
  std::vector<sparse_entry>::const_iterator elementIter, rowEnd;
  sparse_nonzero_element currentElement;

  inline void next() {
    if (end)
      return;
    if (this->matrix == nullptr)
      return;
    this->elementIter++;
    wrapAround();
    updateValue();
  }

  inline void wrapAround() {
    while (this->elementIter == this->rowEnd) {
      if (this->row + 1 < this->matrix->rows) {
        this->row++;
        this->elementIter = this->matrix->row_data.at(this->row).cbegin();
        this->rowEnd = this->matrix->row_data.at(this->row).cend();
      } else {
        this->end = true;
        return;
      }
    }
  }

  inline void updateValue() {
    if (this->end) {
      this->currentElement = sparse_nonzero_element{};
      return;
    }
    const sparse_entry &en = *elementIter;
    this->currentElement =
        sparse_nonzero_element{this->row, en.column, en.value};
  }

  inline bool operator==(const sparse_iterator &other) const {
    if (other.end == this->end) {
      return true;
    }
    return this->matrix == other.matrix && this->row == other.row &&
           this->elementIter == other.elementIter;
  }

  inline bool operator!=(const sparse_iterator &other) const {
    return !((*this) == other);
  }

  inline sparse_nonzero_element &operator*() { return this->currentElement; }

  inline sparse_nonzero_element &operator->() { return **this; }

  inline sparse_iterator &operator++() {
    this->next();
    return *this;
  }

  inline sparse_iterator &operator++(int _) {
    this->next();
    return *this;
  }
};

/// Construct a sparse_iterator for a given smatrix
inline sparse_iterator begin(const smatrix &m) {
  auto it = sparse_iterator{false,
                            &m,
                            0,
                            m.row_data[0].begin(),
                            m.row_data[0].end(),
                            sparse_nonzero_element{}};
  it.wrapAround();
  it.updateValue();
  return it;
}

/// Constructs an end iterator for a given smatrix
inline sparse_iterator end(const smatrix &m) {
  return sparse_iterator{true, &m, m.rows};
}

inline bool operator==(const sparse_entry &a, const sparse_entry &b) {
  return a.column == b.column && a.value == b.value;
}

inline bool operator==(const smatrix &a, const smatrix &b) {
  return a.rows == b.rows && a.cols == b.cols && a.row_data == b.row_data;
}

inline bool operator!=(const smatrix &a, const smatrix &b) { return !(a == b); }

inline smatrix operator+(const smatrix &a, const smatrix &b) {
  if (a.cols != b.cols || a.rows != b.rows) {
    throw std::invalid_argument("Trying to add matrices of different sizes");
  }
  smatrix r{a.rows, a.cols};
  for (int row = 0; row < a.rows; row++) {
    auto &rrow = r.row_data.at(row);
    auto &arow = a.row_data.at(row);
    auto &brow = b.row_data.at(row);
    rrow.reserve(
        std::min(arow.size() + brow.size(), static_cast<size_t>(a.cols)));
    auto m1col = arow.begin();
    auto m2col = brow.begin();
    auto m1end = arow.end();
    auto m2end = brow.end();
    while (!(m1col == m1end && m2col == m2end)) {
      if (m1col == m1end) {
        rrow.insert(rrow.end(), m2col, m2end);
        break;
      } else if (m2col == m2end) {
        rrow.insert(rrow.end(), m1col, m1end);
        break;
      } else {
        if (m1col->column < m2col->column) {
          rrow.push_back(sparse_entry{m1col->column, m1col->value});
          m1col++;
        } else if (m1col->column > m2col->column) {
          rrow.push_back(sparse_entry{m2col->column, m2col->value});
          m2col++;
        } else {
          rrow.push_back(
              sparse_entry{m1col->column, m1col->value + m2col->value});
          m1col++;
          m2col++;
        }
      }
    }
  }

  return r;
}

inline smatrix operator-(const smatrix &a, const smatrix &b) {
  if (a.cols != b.cols || a.rows != b.rows) {
    throw std::invalid_argument(
        "Trying to subtract matrices of different sizes");
  }
  smatrix r{a.rows, a.cols};
  for (int row = 0; row < a.rows; row++) {
    auto &rrow = r.row_data.at(row);
    auto &arow = a.row_data.at(row);
    auto &brow = b.row_data.at(row);
    rrow.reserve(
        std::min(arow.size() + brow.size(), static_cast<size_t>(a.cols)));
    auto m1col = arow.begin();
    auto m2col = brow.begin();
    auto m1end = arow.end();
    auto m2end = brow.end();
    while (!(m1col == m1end && m2col == m2end)) {
      if (m1col == m1end) {
        std::transform(m2col, m2end, std::back_inserter(rrow),
                       [](const sparse_entry &a) {
                         return sparse_entry{a.column, -a.value};
                       });
        break;
      } else if (m2col == m2end) {
        rrow.insert(rrow.end(), m1col, m1end);
        break;
      } else {
        if (m1col->column < m2col->column) {
          rrow.push_back(sparse_entry{m1col->column, m1col->value});
          m1col++;
        } else if (m1col->column > m2col->column) {
          rrow.push_back(sparse_entry{m2col->column, -m2col->value});
          m2col++;
        } else {
          rrow.push_back(
              sparse_entry{m1col->column, m1col->value - m2col->value});
          m1col++;
          m2col++;
        }
      }
    }
  }
  return r;
}

inline smatrix operator-(const smatrix &a) {
  return smatrix::zero(a.rows, a.cols) - a;
}

inline smatrix operator*(const smatrix &a, complex b) {
  smatrix r{a};
  for (auto &row : r.row_data) {
    for (auto &el : row) {
      el.value *= b;
    }
  }
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
inline smatrix operator*(const smatrix &a, const smatrix &b) {
  if (a.cols != b.rows) {
    throw std::invalid_argument(
        "Mismatched dimensions for matrix multiplication");
  }
  smatrix r{a.rows, b.cols};
  smatrix bT{b.T()};
  for (int row = 0; row < r.rows; row++) {
    auto &rrow = r.row_data.at(row);
    auto &arow = a.row_data.at(row);
    rrow.reserve(std::min(arow.size(), (size_t)r.cols));
    for (int col = 0; col < r.cols; col++) {
      complex sum{0.0};
      auto &brow = bT.row_data.at(col);
      auto m1col = arow.begin();
      auto m2col = brow.begin();
      auto m1end = arow.end();
      auto m2end = brow.end();
      while (!(m1col == m1end || m2col == m2end)) {
        if (m1col->column < m2col->column) {
          m1col++;
        } else if (m1col->column > m2col->column) {
          m2col++;
        } else {
          sum += m1col->value * m2col->value;
          m1col++;
          m2col++;
        }
      }
      if (sum != 0.0) {
        rrow.push_back(sparse_entry{col, sum});
      }
    }
  }
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
  return a * b.H();
}

template <>
inline smatrix kronecker<smatrix, gsl::dynamic_extent>(
    gsl::span<const smatrix *, gsl::dynamic_extent> mats) {
  if (mats.size() == 0) {
    return smatrix{1, 1};
  } else if (mats.size() == 1) {
    return smatrix{*mats[0]};
  }
  const int numMats = mats.size();
  int totalRows =
      std::accumulate(mats.cbegin(), mats.cend(), 1,
                      [](int acc, const smatrix *m) { return acc * m->rows; });
  int totalCols =
      std::accumulate(mats.cbegin(), mats.cend(), 1,
                      [](int acc, const smatrix *m) { return acc * m->cols; });
  using row_iter = std::vector<sparse_entry>::iterator;
  smatrix kp{totalRows, totalCols};
  std::vector<sparse_iterator> iterators, endIterators, beginIterators;
  std::vector<int> rowMultipliers(numMats, 1), colMultipliers(numMats, 1);
  beginIterators.reserve(numMats);
  endIterators.reserve(numMats);
  uint64_t elements{1};
  for (int i = 0; i < numMats; i++) {
    beginIterators.push_back(begin(*mats[i]));
    endIterators.push_back(end(*mats[i]));
    elements *= mats[i]->count_nonzero() / mats[i]->rows;
  }
  for (int i = 0; i < totalRows; i++) {
    kp.row_data.at(i).reserve(elements);
  }
  for (int i = numMats - 2; i >= 0; i--) {
    rowMultipliers[i] = rowMultipliers[i + 1] * mats[i + 1]->rows;
    colMultipliers[i] = colMultipliers[i + 1] * mats[i + 1]->cols;
  }
  iterators = beginIterators;
  while (iterators[0] != endIterators[0]) {
    int targetRow{0}, targetCol{0};
    complex product{1.0};
    for (int i = 0; i < numMats; i++) {
      sparse_nonzero_element el = *iterators[i];
      targetRow += rowMultipliers[i] * el.row;
      targetCol += colMultipliers[i] * el.column;
      product *= el.value;
    }
    if (product != 0.0) {
      kp.row_data.at(targetRow).push_back(sparse_entry{targetCol, product});
    }
    for (int i = numMats - 1; i >= 0; i--) {
      iterators[i]++;
      if (iterators[i] == endIterators[i] && i > 0) {
        iterators[i] = beginIterators[i];
      } else {
        break;
      }
    }
  }
  kp.fix_column_order();
  return kp;
}

} // namespace qc
