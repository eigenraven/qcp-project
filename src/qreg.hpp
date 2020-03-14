#pragma once

#include "numeric/numeric.hpp"
#include <functional>
#include <qgate.hpp>
#include <random>
#include <vector>
#include <ctime>

namespace qc {

class QRegister {
public:
  int nqubits;

  inline QRegister() : QRegister(1) {}
  inline QRegister(int nqubits) : nqubits(nqubits) {}

  virtual void applyOperators(gsl::span<std::pair<int, QGate *>> operators,
                              bool disableGrouping) = 0;
  virtual std::vector<int> measureState() = 0;
  virtual std::vector<double> measureMultiple(int shots) = 0;
};

template <class M> class QRegisterImpl : public QRegister {
public:
  M state;
  inline QRegisterImpl(int nqubits)
      : QRegister(nqubits), state(1 << nqubits, 1) {
    state(0, 0) = 1;
  }

public:
  void applyOperators(gsl::span<std::pair<int, QGate *>> operators,
                      bool disableGrouping) final override;

  std::vector<int> measureState() final override;

  std::vector<double> measureMultiple(int shots) final override;
};

template <class M>
void QRegisterImpl<M>::applyOperators(
    gsl::span<std::pair<int, QGate *>> operators, bool disableGrouping) {
  std::vector<const M *> matrices;
  matrices.reserve(this->nqubits);
  std::vector<bool> covered(nqubits, false);
  const M *IDmatrix = &ID.matrix<M>();
  while (operators.size() > 0) {
    auto nonOverlappingEnd = operators.end();
    std::fill(covered.begin(), covered.end(), false);
    int grouped = 0;
    for (auto it = operators.begin(); it != operators.end(); it++) {
      auto [qbit, gate] = *it;
      bool overlaps = std::any_of(covered.begin() + qbit,
                                  covered.begin() + qbit + gate->qubits,
                                  [](bool b) { return b; });
      if (overlaps || (disableGrouping && grouped == 1)) {
        nonOverlappingEnd = it;
        break;
      }
      std::fill(covered.begin() + qbit, covered.begin() + qbit + gate->qubits,
                true);
      grouped++;
    }
    matrices.clear();
    for (int i = 0; i < nqubits; i++) {
      if (!covered.at(i)) {
        matrices.push_back(IDmatrix);
      } else {
        auto [qbit, gate] = *std::find_if(
            operators.begin(), nonOverlappingEnd,
            [i](const std::pair<int, QGate *> &a) { return a.first == i; });
        assert(i == qbit);
        matrices.push_back(&gate->template matrix<M>());
        i += gate->qubits - 1;
      }
    }
    M quantumGate = kronecker<M>(gsl::make_span(matrices));
    state = quantumGate * state;
    if (nonOverlappingEnd == operators.end()) {
      break;
    } else {
      int removed = nonOverlappingEnd - operators.begin();
      operators = operators.subspan(removed, operators.size() - removed);
    }
  }
}

template <class M> std::vector<int> QRegisterImpl<M>::measureState() {
  std::vector<double> norm(state.rows);
  norm.reserve(state.rows);
  for (int row = 0; row < state.rows; row++)
    norm.push_back(std::norm((complex)state(row, 0)));
  static std::random_device rd;
  static std::mt19937 gen(rd() ^ (int)std::time(NULL));
  static std::uniform_real_distribution<> dis(0.0, 1.0);
  double random = dis(gen);
  int state = 0;
  for (int i = 0; i < norm.size(); i++) {
    random -= norm[i];
    if (random <= 0 || i == norm.size() - 1) {
      state = i;
      break;
    }
  }
  std::vector<int> states;
  states.reserve(nqubits);
  for (int i = 0; i < nqubits; i++) {
    states.push_back((state >> (nqubits - 1 - i)) & 1);
  }
  return states;
}

template <class M>
std::vector<double> QRegisterImpl<M>::measureMultiple(int shots) {
  std::vector<double> result(nqubits);
  for (int i = 0; i < shots; i++) {
    std::vector<int> state = measureState();
    for (int j = 0; j < state.size(); j++) {
      result[j] += state[j] / (double)shots;
    }
  }
  return result;
}

} // namespace qc
