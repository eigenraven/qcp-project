#pragma once

#include "numeric/numeric.hpp"
#include <gsl/gsl>
#include <math.h>
#include <string>

namespace qc {

class QGate {
public:
  int qubits;
  const dmatrix dense_matrix;
  const smatrix sparse_matrix;
  inline QGate(int qubits, std::initializer_list<complex> cdata)
      : qubits(qubits), dense_matrix(1 << qubits, 1 << qubits, cdata),
        sparse_matrix(1 << qubits, 1 << qubits, cdata) {}

  template <class M> const M &matrix() const;
};

template <> inline const dmatrix &QGate::matrix<dmatrix>() const {
  return dense_matrix;
}
template <> inline const smatrix &QGate::matrix<smatrix>() const {
  return sparse_matrix;
}

std::optional<QGate> getGate(std::string gate);

extern QGate ID;
extern QGate X;
extern QGate Y;
extern QGate Z;
extern QGate H;
extern QGate CNOT;
extern QGate CY;
extern QGate CZ;
extern QGate SWAP;
extern QGate CCNOT;
} // namespace qc
