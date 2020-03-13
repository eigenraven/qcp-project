#include "numeric/numeric.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <random>

using namespace testing;
using namespace qc;

namespace qc {
std::ostream &operator<<(std::ostream &os, const dmatrix &m) {
  os << "dmatrix{" << ::testing::PrintToString(m.data) << "}";
  return os;
}

std::ostream &operator<<(std::ostream &os, const smatrix &m) {
  os << "smatrix{" << ::testing::PrintToString(m.to_std_vector()) << "}";
  return os;
}
} // namespace qc

TEST(numeric, sanity_check) { EXPECT_EQ(1, 1); }

template <typename M> class MatrixTest : public ::testing::Test {
public:
  typedef M Matrix;
};

using MatrixTypes = ::testing::Types<dmatrix, smatrix>;
TYPED_TEST_SUITE(MatrixTest, MatrixTypes);

TEST(dmatrix, dmatrix_construction) {
  using Matrix = dmatrix;
  Matrix zero11{1, 1};
  EXPECT_EQ(zero11.rows, 1);
  EXPECT_EQ(zero11.cols, 1);
  EXPECT_THAT(zero11.data, ElementsAre(0.0));

  Matrix zero_e11 = Matrix::zero(1, 1);
  EXPECT_EQ(zero_e11.rows, 1);
  EXPECT_EQ(zero_e11.cols, 1);
  EXPECT_THAT(zero_e11.data, ElementsAre(0.0));

  Matrix one_e11 = Matrix::identity(1, 1);
  EXPECT_EQ(one_e11.rows, 1);
  EXPECT_EQ(one_e11.cols, 1);
  EXPECT_THAT(one_e11.data, ElementsAre(1.0));

  Matrix zero83{8, 3};
  EXPECT_EQ(zero83.rows, 8);
  EXPECT_EQ(zero83.cols, 3);
  EXPECT_EQ(zero83.data.size(), 24);
  EXPECT_THAT(zero83.data, Each(Eq(0.0)));

  Matrix m{2, 2, {1.0, 2.0, 3.0, 4.0}};
  EXPECT_EQ(m.rows, 2);
  EXPECT_EQ(m.cols, 2);
  EXPECT_EQ(m(0, 0), 1.0);
  EXPECT_EQ(m(0, 1), 2.0);
  EXPECT_EQ(m(1, 0), 3.0);
  EXPECT_EQ(m(1, 1), 4.0);
  EXPECT_ANY_THROW(m(2, 1));
  EXPECT_ANY_THROW(m(1, 2));
}

TYPED_TEST(MatrixTest, Matrix_transposes) {
  using Matrix = typename TestFixture::Matrix;
  Matrix m{2, 2, {1.0 + 1.0_i, 2.0 + 2.0_i, 3.0 + 3.0_i, 4.0 + 4.0_i}};
  Matrix t{m.T()};
  Matrix h{m.H()};
  EXPECT_EQ(
      t, (Matrix{2, 2, {1.0 + 1.0_i, 3.0 + 3.0_i, 2.0 + 2.0_i, 4.0 + 4.0_i}}));
  EXPECT_EQ(
      h, (Matrix{2, 2, {1.0 - 1.0_i, 3.0 - 3.0_i, 2.0 - 2.0_i, 4.0 - 4.0_i}}));
}

TYPED_TEST(MatrixTest, Matrix_equality) {
  using Matrix = typename TestFixture::Matrix;
  Matrix m1{2, 2, {1.0 + 1.0_i, 2.0 + 2.0_i, 3.0 + 3.0_i, 4.0 + 4.0_i}};
  Matrix m2{2, 2, {1.0 + 1.0_i, 2.0 + 2.0_i, 3.0 + 3.0_i, 4.0 + 4.0_i}};
  Matrix id2 = Matrix::identity(2, 2);
  Matrix id3 = Matrix::identity(3, 3);
  EXPECT_EQ(m1, m1);
  EXPECT_EQ(m2, m2);
  EXPECT_EQ(m1, m2);
  EXPECT_EQ(m2, m1);
  EXPECT_EQ(id2, id2);
  EXPECT_EQ(id3, id3);
  EXPECT_NE(m1, id2);
  EXPECT_NE(m1, id3);
  EXPECT_NE(id2, id3);
}

TYPED_TEST(MatrixTest, Matrix_add) {
  using Matrix = typename TestFixture::Matrix;
  Matrix m{2, 2, {1.0 + 1.0_i, 2.0 + 2.0_i, 3.0 + 3.0_i, 4.0 + 4.0_i}};
  Matrix id = Matrix::identity(2, 2);
  Matrix m_plus_id = m + id;
  EXPECT_THAT(m_plus_id.to_std_vector(),
              ElementsAre(2.0 + 1.0_i, 2.0 + 2.0_i, 3.0 + 3.0_i, 5.0 + 4.0_i));
  EXPECT_ANY_THROW(m + Matrix::zero(3, 3));
}

TYPED_TEST(MatrixTest, Matrix_sub) {
  using Matrix = typename TestFixture::Matrix;
  Matrix m{2, 2, {1.0 + 1.0_i, 2.0 + 2.0_i, 3.0 + 3.0_i, 4.0 + 4.0_i}};
  Matrix id = Matrix::identity(2, 2);
  Matrix m_sub_id = m - id;
  EXPECT_THAT(m_sub_id.to_std_vector(),
              ElementsAre(0.0 + 1.0_i, 2.0 + 2.0_i, 3.0 + 3.0_i, 3.0 + 4.0_i));
  EXPECT_ANY_THROW(m - Matrix::zero(3, 3));
}

TYPED_TEST(MatrixTest, Matrix_neg) {
  using Matrix = typename TestFixture::Matrix;
  Matrix m{2, 2, {1.0 + 1.0_i, 2.0 + 2.0_i, 3.0 + 3.0_i, 4.0 + 4.0_i}};
  Matrix m_neg = -m;
  EXPECT_THAT(m_neg.to_std_vector(), ElementsAre(-1.0 - 1.0_i, -2.0 - 2.0_i,
                                                 -3.0 - 3.0_i, -4.0 - 4.0_i));
}

TYPED_TEST(MatrixTest, Matrix_mulconst) {
  using Matrix = typename TestFixture::Matrix;
  Matrix m{2, 2, {1.0 + 1.0_i, 2.0 + 2.0_i, 3.0 + 3.0_i, 4.0 + 4.0_i}};
  Matrix m_neg = m * (-1.0 + 0.0_i);
  EXPECT_THAT(m_neg.to_std_vector(), ElementsAre(-1.0 - 1.0_i, -2.0 - 2.0_i,
                                                 -3.0 - 3.0_i, -4.0 - 4.0_i));
  EXPECT_EQ(m, m * 1.0);
  EXPECT_EQ(m, 1.0 * m);
  EXPECT_EQ(m * -1.0, m * (-1.0 + 0.0_i));
}

TYPED_TEST(MatrixTest, make_dvector) {
  using Matrix = typename TestFixture::Matrix;
  EXPECT_EQ((make_vector<Matrix>({1.0, 2.0, 3.0})),
            (Matrix{3, 1, {1.0, 2.0, 3.0}}));
  EXPECT_EQ((make_covector<Matrix>({1.0, 2.0, 3.0})),
            (Matrix{1, 3, {1.0, 2.0, 3.0}}));
}

TYPED_TEST(MatrixTest, vector_products) {
  using Matrix = typename TestFixture::Matrix;
  Matrix a = make_vector<Matrix>({1.0, 1.0_i});
  Matrix b = make_vector<Matrix>({0.0, 1.0});
  EXPECT_EQ(dot(a, b), -1.0_i);
  EXPECT_EQ(dot(b, a), 1.0_i);
  EXPECT_EQ(outer(a, b), (Matrix{2, 2, {0.0, 1.0, 0.0, 1.0_i}}));
  EXPECT_EQ(outer(b, a), (Matrix{2, 2, {0.0, 0.0, 1.0, -1.0_i}}));
}

TYPED_TEST(MatrixTest, matrix_multiply_square) {
  using Matrix = typename TestFixture::Matrix;
  Matrix id = Matrix::identity(2, 2);
  Matrix diag2{2, 2, {2.0, 0.0, 0.0, 2.0}};
  Matrix m{2, 2, {1.0, 1.0_i, 2.0, 3.0}};
  EXPECT_EQ(id * diag2, diag2);
  EXPECT_EQ(diag2 * id, diag2);
  EXPECT_EQ(id * m, m);
  EXPECT_EQ(m * id, m);
  EXPECT_EQ(id * id, id);
  EXPECT_EQ(m * m, (Matrix{2, 2, {1.0 + 2.0_i, 4.0_i, 8.0, 9.0 + 2.0_i}}));
}

TYPED_TEST(MatrixTest, kronecker_product) {
  using Matrix = typename TestFixture::Matrix;
  Matrix id2 = Matrix::identity(2, 2);
  Matrix krid2 = kronecker({&id2, &id2});
  EXPECT_EQ(krid2, Matrix::identity(4, 4));
  {
    std::vector<const Matrix *> id2list;
    id2list.push_back(&id2);
    id2list.push_back(&id2);
    Matrix krid2_fromvec = kronecker(gsl::make_span(id2list));
    EXPECT_EQ(krid2_fromvec, Matrix::identity(4, 4));
  }
}

TEST(MatrixTest, sparse_iterator_test) {
  smatrix zeros = smatrix::zero(10, 10);
  smatrix ones = smatrix::identity(10, 10);
  EXPECT_EQ(begin(zeros), end(zeros));
  EXPECT_NE(begin(ones), end(ones));
  sparse_iterator oneBegin{begin(ones)}, oneEnd{end(ones)};
  for (int i = 0; i < 10; i++) {
    EXPECT_NE(oneBegin, oneEnd);
    sparse_nonzero_element val = *oneBegin;
    EXPECT_EQ(val.row, val.column);
    EXPECT_EQ(val.row, i);
    EXPECT_EQ(val.value, 1.0);
    oneBegin++;
  }
  EXPECT_EQ(oneBegin, oneEnd);
}

TEST(MatrixTest, RandomKPEquivalence) {
  std::mt19937 rng{738643};
  std::uniform_int_distribution<> elements_dist{5, 40};
  std::uniform_int_distribution<> position_dist{0, 6};
  std::uniform_real_distribution<> values_dist{-100.0f, 100.0f};
  std::vector<smatrix> S;
  std::vector<dmatrix> D;
  S.resize(3, smatrix::zero(7, 7));
  D.resize(3, dmatrix::zero(7, 7));
  for (int _trial = 0; _trial < 64; _trial++) {
    std::fill(S.begin(), S.end(), smatrix::zero(7, 7));
    std::fill(D.begin(), D.end(), dmatrix::zero(7, 7));
    for (int mat = 0; mat < S.size(); mat++) {
      int els = elements_dist(rng);
      for (int el = 0; el < els; el++) {
        int row = position_dist(rng);
        int col = position_dist(rng);
        complex val = values_dist(rng) + 1.0_i * values_dist(rng);
        S[mat](row, col) = val;
        D[mat](row, col) = val;
      }
    }
    smatrix skp = kronecker({&S[0], &S[1], &S[2]});
    dmatrix dkp = kronecker({&D[0], &D[1], &D[2]});
    dmatrix skp_d = convert_sparse_to_dense(skp);
    ASSERT_EQ(dkp, skp_d);
  }
}
