
#include <qcsim.hpp>

namespace qc {

std::pair<int, int> linked_version() {
  return std::make_pair(QCSIM_VERSION_MAJOR, QCSIM_VERSION_MINOR);
}

} // namespace qc
