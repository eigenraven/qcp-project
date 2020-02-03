#include <iostream>
#include <qcsim.hpp>
#include <qreg.hpp>

namespace qc {

std::pair<int, int> linked_version() {
  return std::make_pair(QCSIM_VERSION_MAJOR, QCSIM_VERSION_MINOR);
}

void simulate(int qbits) {
	QRegister reg(3);
	reg.applyOperator(hadamard,1);
}

} // namespace qc
