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
  dmatrix m{2, 2, {1.0 + 1.0i, 2.0 + 2.0i, 3.0 + 3.0i, 4.0 + 4.0i}};
  dmatrix t{m.T()};
  dmatrix h{m.H()};
  EXPECT_THAT(t.data,
              ElementsAre(1.0 + 1.0i, 3.0 + 3.0i, 2.0 + 2.0i, 4.0 + 4.0i));
  EXPECT_THAT(h.data,
              ElementsAre(1.0 - 1.0i, 3.0 - 3.0i, 2.0 - 2.0i, 4.0 - 4.0i));
}
