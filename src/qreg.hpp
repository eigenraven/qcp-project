/**
 * @file qreg.hpp
 * @brief Quantum register implementation.
 */
#pragma once

#include "numeric/numeric.hpp"
#include <ctime>
#include <functional>
#include <iostream>
#include <qgate.hpp>
#include <random>
#include <vector>

namespace qc {

/// A quantum register interface (QRegisterImpl is the implementation parametrized by matrix type)
class QRegister {
public:
  /// Number of qubits in the register
  int nqubits;

  inline QRegister() : QRegister(1) {}
  inline QRegister(int nqubits) : nqubits(nqubits) {}

  /// Applies a given span of operators to the paired qubits
  virtual void applyOperators(gsl::span<std::pair<int, QGate *>> operators,
                              bool disableGrouping, double noise) = 0;
  /// Applies quantum noise to the register
  virtual void applyNoise(double noise) = 0;
  /// Resets the state to |00..0>
  virtual void reset() = 0;
  /// Measures the qubit probabilities randomly
  virtual std::vector<int> measureQubits() = 0;
  /// Measures the state of the register
  virtual int measureState() = 0;
  /// Performs a full simulation
  virtual std::vector<double>
  simulate(gsl::span<std::pair<int, QGate *>> operators, int shots,
           bool disableGrouping, double noise, bool states, bool phase) = 0;
};

/// Implementation of QRegister for a given matrix type
template <class M> class QRegisterImpl : public QRegister {
public:
  M state;
  inline QRegisterImpl(int nqubits)
      : QRegister(nqubits), state(1 << nqubits, 1) {
    state(0, 0) = 1;
  }

public:
  void applyOperators(gsl::span<std::pair<int, QGate *>> operators,
                      bool disableGrouping, double noise) final override;

  void applyNoise(double noise) final override;
  void reset() final override;

  std::vector<int> measureQubits() final override;
  int measureState() final override;

  std::vector<double> simulate(gsl::span<std::pair<int, QGate *>> operators,
                               int shots, bool disableGrouping, double noise,
                               bool states, bool phase) final override;
};

template <class M> void QRegisterImpl<M>::reset() {
  state = M(1 << nqubits, 1);
  state(0, 0) = 1;
}

template <class M>
void QRegisterImpl<M>::applyOperators(
    gsl::span<std::pair<int, QGate *>> operators, bool disableGrouping,
    double noise) {
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
      std::fill(covered.begin() + qbit, covered.begin() + qbit + gate->qubits,true);
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
    if (noise)
      applyNoise(noise);
  }
}

template <class M> void QRegisterImpl<M>::applyNoise(double noise) {
  static std::random_device rd;
  static std::mt19937 gen(rd() ^ (int)std::time(NULL));
  static std::uniform_real_distribution<> dis(0.0, 1.0);
  for (int i = 0; i < nqubits; i++) {
    if (dis(gen) < noise) {
      for (int j = 0; j < state.rows; j++) {
        if (j >> i & 1) {
          state(j & ~(1 << i), 0) = std::polar(
              sqrt(pow(std::abs((complex)state(j & ~(1 << i), 0)), 2) +
                   pow(std::abs((complex)state(j, 0)), 2)),
              std::arg((complex)state(j & ~(1 << i), 0)));
        }
      }
    }
  }
}

template <class M> std::vector<int> QRegisterImpl<M>::measureQubits() {
  int state = measureState();
  std::vector<int> states;
  states.reserve(nqubits);
  for (int i = 0; i < nqubits; i++) {
    states.push_back((state >> (nqubits - 1 - i)) & 1);
  }
  return states;
}

template <class M> int QRegisterImpl<M>::measureState() {
  std::vector<double> norm;
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
  return state;
}

template <class M>
std::vector<double>
QRegisterImpl<M>::simulate(gsl::span<std::pair<int, QGate *>> operators,
                           int shots, bool disableGrouping, double noise,
                           bool states, bool phase) {
  std::vector<double> result;
  if (phase) {
    result = std::vector<double>(2 * nqubits);
  } else if (states) {
    result = std::vector<double>(1 << nqubits);
  } else {
    result = std::vector<double>(nqubits);
  }
  if (phase || !noise) {
    applyOperators(operators, disableGrouping, noise);
  }
  if (phase) {
    for (int row = 0; row < state.rows; row++) {
      complex z = state(row, 0);
      result.push_back(std::real(z));
      result.push_back(std::imag(z));
    }
  } else {
    for (int i = 0; i < shots; i++) {
      if (noise) {
        reset();
        applyOperators(operators, disableGrouping, noise);
      }
      if (states) {
        result[measureState()] += 1 / (double)shots;
      } else {
        std::vector<int> state = measureQubits();
        for (int j = 0; j < state.size(); j++) {
          result[j] += state[j] / (double)shots;
        }
      }
    }
  }
  return result;
}

} // namespace qc
