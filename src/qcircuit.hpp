/**
 * @file qcircuit.hpp
 * @brief Quantum circuit model.
 */
#pragma once

#include <algorithm>
#include <iostream>
#include <qreg.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace qc {

/// A quantum circuit object
class QCircuit {
  /// The associated quantum register
  std::unique_ptr<QRegister> qreg;
  /// The array of gates together with the first qubit they are applied to
  std::vector<std::pair<int, QGate *>> gates;

public:
  /// Constructor with a given matrix type (because C++ doesn't allow templated
  /// constructors to work with manually specified types)
  template <class M> inline static std::unique_ptr<QCircuit> make(int qubits) {
    std::unique_ptr<QCircuit> r = std::make_unique<QCircuit>();
    r->qreg = std::make_unique<QRegisterImpl<M>>(qubits);
    return r;
  }

  /// Resets the register state to |00..0>
  inline void reset() { qreg->reset(); }

  /// @{
  /// Appends gates to the end of the circuit
  inline void nop(int qubit) { singleGate(&ID, qubit); }

  inline void h(int qubit) { singleGate(&H, qubit); }

  inline void x(int qubit) { singleGate(&X, qubit); }

  inline void y(int qubit) { singleGate(&Y, qubit); }

  inline void z(int qubit) { singleGate(&Z, qubit); }

  inline void s(int qubit) { singleGate(&S, qubit); }

  inline void t(int qubit) { singleGate(&T, qubit); }

  inline void tinv(int qubit) { singleGate(&Tinv, qubit); }

  inline void v(int qubit) { singleGate(&V, qubit); }

  inline void vinv(int qubit) { singleGate(&Vinv, qubit); }

  inline void cnot(int qubit1, int qubit2) {
    doubleGate(&CNOT, qubit1, qubit2);
  }

  inline void swap(int qubit1, int qubit2) {
    doubleGate(&SWAP, qubit1, qubit2);
  }

  void cswap(int qubit1, int qubit2, int qubit3) {
    multipleGate(&CSWAP, {qubit1, qubit2, qubit3});
  }

  void cv(int qubit1, int qubit2) {
    tinv(qubit1);
    h(qubit2);
    cnot(qubit2, qubit1);
    t(qubit1);
    tinv(qubit2);
    cnot(qubit2, qubit1);
    h(qubit2);
  }

  inline void cvinv(int qubit1, int qubit2) {
    h(qubit2);
    cnot(qubit2, qubit1);
    tinv(qubit1);
    t(qubit2);
    cnot(qubit2, qubit1);
    t(qubit1);
    h(qubit2);
  }

  inline void ccnot(int qubit1, int qubit2, int qubit3) {
    multipleGate(&CCNOT, {qubit1, qubit2, qubit3});
  }
  /// @}

  /// Adds a single-input gate to the circuit
  inline void singleGate(QGate *gate, int qubit) {
    if (gate->qubits != 1)
      throw std::invalid_argument("Gate must operate on one qubit");
    gates.push_back(std::make_pair(qubit, gate));
  }

  /// Adds a gate copied to act on every qubit
  void gateAll(QGate *gate) {
    for (int i = 0; i < qreg->nqubits; i++) {
      singleGate(gate, i);
    }
  }

  /// Adds a two-input gate to the circuit
  inline void doubleGate(QGate *gate, int qubit1, int qubit2) {
    if (gate->qubits != 2)
      throw std::invalid_argument("This gate does not operate on two qubits");
    if (qubit1 == qubit2)
      throw std::invalid_argument("This gate must operate on different qubits");
    int lowest = std::min(qubit1, qubit2);
    int highest = std::max(qubit1, qubit2);
    for (int i = highest - 1; i > lowest; i--) {
      swapNext(i);
    }
    if (qubit2 > qubit1) {
      gates.push_back(std::make_pair(lowest, gate));
    } else {
      swapNext(lowest);
      gates.push_back(std::make_pair(lowest, gate));
      swapNext(lowest);
    }
    for (int i = lowest + 1; i < highest; i++) {
      swapNext(i);
    }
  }

  /// Adds an arbitrary gate to the circuit acting on any set of qubits
  inline void multipleGate(QGate *gate, std::vector<int> qubits) {
    if (gate->qubits != qubits.size())
      throw std::invalid_argument(
          "This gate does not operate on the correct number of qubits");
    int lowest = qubits[0];
    std::vector<int> overlaps(qreg->nqubits);
    for (int i = 0; i < qubits.size(); i++) {
      overlaps[qubits[i]] = qubits[i];
      lowest = std::min(lowest, qubits[i]);
      for (int j = i + 1; j < qubits.size(); j++) {
        if (qubits[i] == qubits[j])
          throw std::invalid_argument(
              "This gate must operate on different qubits");
      }
    }
    for (int i = 0; i < qubits.size(); i++) {
      if (overlaps[qubits[i]] != lowest + i) {
        swap(overlaps[qubits[i]], lowest + i);
        overlaps[lowest + i] = qubits[i];
      }
    }
    gates.push_back(std::make_pair(lowest, gate));
    for (int i = qubits.size() - 1; i >= 0; i--) {
      if (overlaps[qubits[i]] != lowest + i) {
        swap(overlaps[qubits[i]], lowest + i);
        overlaps[lowest + i] = qubits[i];
      }
    }
  }

  /// Adds a swap gate
  inline void swapNext(int qubit) {
    gates.push_back(std::make_pair(qubit, &SWAP));
  }

  /// Performs a circuit simulation, returning the determined probabilities of
  /// states
  inline std::vector<double> simulate(int shots, bool disableGrouping = false,
                                      double noise = 0.0, bool states = false) {
    return qreg->simulate(gsl::make_span(gates), shots, disableGrouping, noise,
                          states);
  }

  /// Returns raw state probabilities
  std::string print(std::vector<double> result, bool states) {
    std::ostringstream s;
    for (int i = 0; i < result.size(); i++) {
      if (states) {
        s << "|" << binary(i, qreg->nqubits) << ">: " << result[i] << "\n";
      } else {
        s << "QBit " << i << ": " << result[i] << "\n";
      }
    }
    return s.str();
  }
};
} // namespace qc
