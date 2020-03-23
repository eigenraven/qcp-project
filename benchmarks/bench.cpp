#include "numeric/numeric.hpp"
#include <benchmark/benchmark.h>
#include <random>
#include <unordered_set>
#include <utility>

using namespace std::string_literals;
using namespace qc;

namespace {
thread_local std::mt19937 rng{738643};

template <class M> M matrix_with_random_data(int rows_cols, int elements) {
  elements = std::min(elements, rows_cols * rows_cols);
  std::uniform_int_distribution<> position_dist{0, rows_cols - 1};
  std::uniform_real_distribution<> values_dist{-100.0, 100.0};
  M matrix = M::zero(rows_cols, rows_cols);
  std::unordered_set<int> placedElements;
  for (int el = 0; el < elements; el++) {
    int row = position_dist(rng);
    int col = position_dist(rng);
    int pos = col + row * rows_cols;
    while (placedElements.count(pos) > 0) {
      col++;
      if (col >= rows_cols) {
        col = 0;
        row++;
        if (row >= rows_cols) {
          row = 0;
        }
      }
      pos = col + row * rows_cols;
    }
    complex val{values_dist(rng), values_dist(rng)};
    matrix(row, col) = val;
  }
  return matrix;
}
} // namespace

static void CustomKroneckerArguments(benchmark::internal::Benchmark *b) {
  for (int size = 2; size <= 64; size *= 2) {
    for (int elements_pct : {25, 50, 75, 100}) {
      b->Args({size, elements_pct});
    }
  }
}

static void BM_Kronecker_Dense_2(benchmark::State &state) {
  int size = state.range(0);
  int elements = state.range(1) * size * size / 100;
  using Matrix = dmatrix;
  Matrix m1 = matrix_with_random_data<Matrix>(size, elements);
  Matrix m2 = matrix_with_random_data<Matrix>(size, elements);
  for (auto _ : state) {
    Matrix kp = kronecker({&m1, &m2});
    benchmark::DoNotOptimize(kp);
  }
}

static void BM_Kronecker_Sparse_2(benchmark::State &state) {
  int size = state.range(0);
  int elements = state.range(1) * size * size / 100;
  using Matrix = smatrix;
  Matrix m1 = matrix_with_random_data<Matrix>(size, elements);
  Matrix m2 = matrix_with_random_data<Matrix>(size, elements);
  for (auto _ : state) {
    Matrix kp = kronecker({&m1, &m2});
    benchmark::DoNotOptimize(kp);
  }
}

BENCHMARK(BM_Kronecker_Dense_2)
    ->ArgNames({"size"s, "nonzero_percent"s})
    ->Apply(CustomKroneckerArguments);
BENCHMARK(BM_Kronecker_Sparse_2)
    ->ArgNames({"size"s, "nonzero_percent"s})
    ->Apply(CustomKroneckerArguments);

static void BM_Matmul_Dense_2(benchmark::State &state) {
  int size = state.range(0);
  int elements = state.range(1) * size * size / 100;
  using Matrix = dmatrix;
  Matrix m1 = matrix_with_random_data<Matrix>(size, elements);
  Matrix m2 = matrix_with_random_data<Matrix>(size, elements);
  for (auto _ : state) {
    Matrix kp = m1 * m2;
    benchmark::DoNotOptimize(kp);
  }
}

static void BM_Matmul_Sparse_2(benchmark::State &state) {
  int size = state.range(0);
  int elements = state.range(1) * size * size / 100;
  using Matrix = smatrix;
  Matrix m1 = matrix_with_random_data<Matrix>(size, elements);
  Matrix m2 = matrix_with_random_data<Matrix>(size, elements);
  for (auto _ : state) {
    Matrix kp = m1 * m2;
    benchmark::DoNotOptimize(kp);
  }
}

BENCHMARK(BM_Matmul_Dense_2)
    ->ArgNames({"size"s, "nonzero_percent"s})
    ->Apply(CustomKroneckerArguments);
BENCHMARK(BM_Matmul_Sparse_2)
    ->ArgNames({"size"s, "nonzero_percent"s})
    ->Apply(CustomKroneckerArguments);

static void CustomKroneckerN2Arguments(benchmark::internal::Benchmark *b) {
  for (int count = 2; count <= 12; count += 1) {
    for (int elements : {2, 3, 4}) {
      b->Args({count, elements});
    }
  }
}

static void BM_Kronecker_Dense_N2(benchmark::State &state) {
  int count = state.range(0);
  int elements = state.range(1);
  using Matrix = dmatrix;
  std::vector<Matrix> matrices;
  std::vector<const Matrix *> matrixPtrs;
  matrices.reserve(count);
  for (int i = 0; i < count; i++) {
    matrices.emplace_back(matrix_with_random_data<Matrix>(2, elements));
    matrixPtrs.emplace_back(&matrices[i]);
  }
  for (auto _ : state) {
    Matrix kp = kronecker(gsl::make_span(matrixPtrs));
    benchmark::DoNotOptimize(kp);
  }
}

static void BM_Kronecker_Sparse_N2(benchmark::State &state) {
  int count = state.range(0);
  int elements = state.range(1);
  using Matrix = smatrix;
  std::vector<Matrix> matrices;
  std::vector<const Matrix *> matrixPtrs;
  matrices.reserve(count);
  for (int i = 0; i < count; i++) {
    matrices.emplace_back(matrix_with_random_data<Matrix>(2, elements));
    matrixPtrs.emplace_back(&matrices[i]);
  }
  for (auto _ : state) {
    Matrix kp = kronecker(gsl::make_span(matrixPtrs));
    benchmark::DoNotOptimize(kp);
  }
}

BENCHMARK(BM_Kronecker_Dense_N2)
    ->ArgNames({"count"s, "nonzero_percent"s})
    ->Apply(CustomKroneckerN2Arguments);
BENCHMARK(BM_Kronecker_Sparse_N2)
    ->ArgNames({"count"s, "nonzero_percent"s})
    ->Apply(CustomKroneckerN2Arguments);

BENCHMARK_MAIN();
