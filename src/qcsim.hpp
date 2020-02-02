#pragma once

#include <utility>

#define QCSIM_VERSION_MAJOR 0
#define QCSIM_VERSION_MINOR 1

namespace qc {

std::pair<int, int> linked_version();
void simulate(int qbits);

} // namespace qc
