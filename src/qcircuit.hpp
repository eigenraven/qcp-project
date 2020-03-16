#pragma once

#include <algorithm>
#include <iostream>
#include <qreg.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace qc {

class QCircuit {
  std::unique_ptr<QRegister> qreg;
  std::vector<std::pair<int, QGate *>> gates;

public:
  template <class M> inline static std::unique_ptr<QCircuit> make(int qubits) {
    std::unique_ptr<QCircuit> r = std::make_unique<QCircuit>();
    r->qreg = std::make_unique<QRegisterImpl<M>>(qubits);
    return r;
  }

  inline void reset() {
    qreg->reset();
  }

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

  inline void cnot(int qubit1, int qubit2) { doubleGate(&CNOT, qubit1, qubit2); }

  inline void swap(int qubit1, int qubit2) { doubleGate(&SWAP, qubit1, qubit2); }

  inline void cv(int qubit1, int qubit2) {
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

  inline void singleGate(QGate *gate, int qubit) {
    if (gate->qubits != 1)
      throw std::invalid_argument("Gate must operate on one qubit");
    gates.push_back(std::make_pair(qubit, gate));
  }

  inline void applyAll(QGate *gate) {
    for (int i = 0; i < qreg->nqubits; i++) {
      singleGate(gate, i);
    }
  }

  // So far only gates that operate on single or multiple consecutive qubits
  // have been implemented So this emulates a quantum computer with the qubits
  // connected linearly i.e. Each qubit is only directly connected to its
  // nearest neighbours
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

  inline void swapNext(int qubit) { gates.push_back(std::make_pair(qubit, &SWAP)); }

  inline std::vector<double> simulate(int shots, bool disableGrouping = false,
                               double noise = 0.0, bool states = false) {
    return qreg->simulate(gsl::make_span(gates), shots, disableGrouping, noise,
                          states);
  }

  inline std::string binary(unsigned x, int size) {
    std::string s;
    do {
      s.push_back('0' + (x & 1));
    } while (x >>= 1);
    std::reverse(s.begin(), s.end());
    s = std::string(size - s.size(), '0') + s;
    return s;
  }

  inline std::string print(std::vector<double> result, bool states) {
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
