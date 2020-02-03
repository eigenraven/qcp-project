#include "numeric.hpp"

namespace qc {

complex dmatrix::determinant() const {
  assert(0);
  return 0.0;
}

std::optional<dmatrix> dmatrix::inverse() const {
  assert(0);
  return std::nullopt;
}

bool dmatrix::is_unitary() const { return this->H() == this->inverse(); }

} // namespace qc
