/**
 * @file This header contains the definitions of all numeric primitives used by
 * the simulator, like vectors, matrices; and operations on them.
 */
#pragma once

#include "common.hpp"
#include "dense_matrix.hpp"
#include "sparse_matrix.hpp"
#include <string>
#include <algorithm>

namespace qc {

inline dmatrix convert_sparse_to_dense(const smatrix &m) {
  dmatrix r{m.rows, m.cols};
  r.data = m.to_std_vector();
  return r;
}

inline smatrix convert_dense_to_sparse(const dmatrix &m) {
  return smatrix{m.rows, m.cols, gsl::make_span(const_cast<dmatrix &>(m).data)};
}

inline std::string binary(unsigned x, int minSize) {
    std::string s;
    do {
        s.push_back('0'+(x&1));
    } while (x>>=1);
    std::reverse(s.begin(), s.end());
	s=std::string(std::max(0,minSize-(int)s.size()),'0')+s;
    return s;
  }
} // namespace qc
