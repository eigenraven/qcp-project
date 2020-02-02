#include "numeric/numeric.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace testing;
using namespace qc;
TEST(numeric, sanity_check) { EXPECT_EQ(1, 1); }

TEST(numeric, dmatrix_construction) {
  dmatrix zero11{1, 1};
  EXPECT_EQ(zero11.rows, 1);
  EXPECT_EQ(zero11.cols, 1);
  EXPECT_THAT(zero11.data, ElementsAre(0.0));

  dmatrix zero_e11 = dmatrix::zero(1, 1);
  EXPECT_EQ(zero_e11.rows, 1);
  EXPECT_EQ(zero_e11.cols, 1);
  EXPECT_THAT(zero_e11.data, ElementsAre(0.0));

  dmatrix one_e11 = dmatrix::identity(1, 1);
  EXPECT_EQ(one_e11.rows, 1);
  EXPECT_EQ(one_e11.cols, 1);
  EXPECT_THAT(one_e11.data, ElementsAre(1.0));

  dmatrix zero83{8, 3};
  EXPECT_EQ(zero83.rows, 8);
  EXPECT_EQ(zero83.cols, 3);
  EXPECT_EQ(zero83.data.size(), 24);
  EXPECT_THAT(zero83.data, Each(Eq(0.0)));

  dmatrix m{2, 2, {1.0, 2.0, 3.0, 4.0}};
  EXPECT_EQ(m.rows, 2);
  EXPECT_EQ(m.cols, 2);
  EXPECT_EQ(m(0, 0), 1.0);
  EXPECT_EQ(m(0, 1), 2.0);
  EXPECT_EQ(m(1, 0), 3.0);
  EXPECT_EQ(m(1, 1), 4.0);
  EXPECT_ANY_THROW(m(2, 1));
  EXPECT_ANY_THROW(m(1, 2));
}

TEST(numeric, dmatrix_transposes) {
  dmatrix m{2, 2, {1.0 + 1.0_i, 2.0 + 2.0_i, 3.0 + 3.0_i, 4.0 + 4.0_i}};
  dmatrix t{m.T()};
  dmatrix h{m.H()};
  EXPECT_THAT(t.data,
              ElementsAre(1.0 + 1.0_i, 3.0 + 3.0_i, 2.0 + 2.0_i, 4.0 + 4.0_i));
  EXPECT_THAT(h.data,
              ElementsAre(1.0 - 1.0_i, 3.0 - 3.0_i, 2.0 - 2.0_i, 4.0 - 4.0_i));
}

TEST(numeric, dmatrix_equality) {
  dmatrix m1{2, 2, {1.0 + 1.0_i, 2.0 + 2.0_i, 3.0 + 3.0_i, 4.0 + 4.0_i}};
  dmatrix m2{2, 2, {1.0 + 1.0_i, 2.0 + 2.0_i, 3.0 + 3.0_i, 4.0 + 4.0_i}};
  dmatrix id2 = dmatrix::identity(2, 2);
  dmatrix id3 = dmatrix::identity(3, 3);
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

TEST(numeric, dmatrix_add) {
  dmatrix m{2, 2, {1.0 + 1.0_i, 2.0 + 2.0_i, 3.0 + 3.0_i, 4.0 + 4.0_i}};
  dmatrix id = dmatrix::identity(2, 2);
  dmatrix m_plus_id = m + id;
  EXPECT_THAT(m_plus_id.data,
              ElementsAre(2.0 + 1.0_i, 2.0 + 2.0_i, 3.0 + 3.0_i, 5.0 + 4.0_i));
  EXPECT_ANY_THROW(m + dmatrix::zero(3, 3));
}

TEST(numeric, dmatrix_sub) {
  dmatrix m{2, 2, {1.0 + 1.0_i, 2.0 + 2.0_i, 3.0 + 3.0_i, 4.0 + 4.0_i}};
  dmatrix id = dmatrix::identity(2, 2);
  dmatrix m_sub_id = m - id;
  EXPECT_THAT(m_sub_id.data,
              ElementsAre(0.0 + 1.0_i, 2.0 + 2.0_i, 3.0 + 3.0_i, 3.0 + 4.0_i));
  EXPECT_ANY_THROW(m - dmatrix::zero(3, 3));
}

TEST(numeric, dmatrix_neg) {
  dmatrix m{2, 2, {1.0 + 1.0_i, 2.0 + 2.0_i, 3.0 + 3.0_i, 4.0 + 4.0_i}};
  dmatrix m_neg = -m;
  EXPECT_THAT(m_neg.data, ElementsAre(-1.0 - 1.0_i, -2.0 - 2.0_i, -3.0 - 3.0_i,
                                      -4.0 - 4.0_i));
}

TEST(numeric, dmatrix_mulconst) {
  dmatrix m{2, 2, {1.0 + 1.0_i, 2.0 + 2.0_i, 3.0 + 3.0_i, 4.0 + 4.0_i}};
  dmatrix m_neg = m * (-1.0 + 0.0_i);
  EXPECT_THAT(m_neg.data, ElementsAre(-1.0 - 1.0_i, -2.0 - 2.0_i, -3.0 - 3.0_i,
                                      -4.0 - 4.0_i));
  EXPECT_EQ(m, m * 1.0);
  EXPECT_EQ(m, 1.0 * m);
  EXPECT_EQ(m * -1.0, m * (-1.0 + 0.0_i));
}

TEST(numeric, make_dvector) {
  EXPECT_EQ((make_dvector({1.0, 2.0, 3.0})), (dmatrix{3, 1, {1.0, 2.0, 3.0}}));
  EXPECT_EQ((make_dcovector({1.0, 2.0, 3.0})),
            (dmatrix{1, 3, {1.0, 2.0, 3.0}}));
}

TEST(numeric, vector_products) {
  dvector a = make_dvector({1.0, 1.0_i});
  dvector b = make_dvector({0.0, 1.0});
  EXPECT_EQ(dot(a, b), -1.0_i);
  EXPECT_EQ(dot(b, a), 1.0_i);
  EXPECT_EQ(outer(a, b), (dmatrix{2, 2, {0.0, 1.0, 0.0, 1.0_i}}));
  EXPECT_EQ(outer(b, a), (dmatrix{2, 2, {0.0, 0.0, 1.0, 1.0_i}}));
}

TEST(numeric, matrix_multiply_square) {
  dmatrix id = dmatrix::identity(2, 2);
  dmatrix diag2{2, 2, {2.0, 0.0, 0.0, 2.0}};
  dmatrix m{2, 2, {1.0, 1.0_i, 2.0, 3.0}};
  EXPECT_EQ(id * diag2, diag2);
  EXPECT_EQ(diag2 * id, diag2);
  EXPECT_EQ(id * m, m);
  EXPECT_EQ(m * id, m);
  EXPECT_EQ(id * id, id);
  EXPECT_EQ(m * m, (dmatrix{2, 2, {1.0 + 2.0_i, 4.0_i, 8.0, 9.0 + 2.0_i}}));
}
