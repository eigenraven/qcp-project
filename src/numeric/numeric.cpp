#include "numeric.hpp"

namespace qc {

complex operator""_i(long double val) {
  return complex{0, static_cast<real>(val)};
}

complex operator""_i(unsigned long long val) {
  return complex{0, static_cast<real>(val)};
}

} // namespace qc
