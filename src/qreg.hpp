#pragma once

#include "numeric/numeric.hpp"
#include <functional>
#include <qgate.hpp>
#include <random>
#include <vector>

namespace qc {

class QRegister {
public:
  int nqubits;
  dmatrix state;
  QRegister(int nqubits) : nqubits(nqubits), state(1 << nqubits, 1) {
    state.data[0] = 1;
  }

public:
  void applyOperator(QGate gate, int qubit);

  std::vector<int> measureState();

  std::vector<double> measureMultiple(int shots);
};
} // namespace qc
