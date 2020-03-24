/**
 * @file qgate.hpp
 * @brief Standard quantum gate definitions.
 */
#pragma once

#include "numeric/numeric.hpp"
#include <gsl/gsl>
#include <math.h>
#include <string>

namespace qc {

/// A quantum gate definition, used for assembling circuits
class QGate {
public:
  /// Number of inputs=outputs of the gate
  int qubits;
  /// Dense matrix representation of the gate
  const dmatrix dense_matrix;
  /// Sparse matrix representation of the gate
  const smatrix sparse_matrix;
  /// Constructor
  inline QGate(int qubits, std::initializer_list<complex> cdata)
      : qubits(qubits), dense_matrix(1 << qubits, 1 << qubits, cdata),
        sparse_matrix(1 << qubits, 1 << qubits, cdata) {}

  /// Matrix-type-parametrized matrix accessor
  template <class M> const M &matrix() const;
};

template <> inline const dmatrix &QGate::matrix<dmatrix>() const {
  return dense_matrix;
}
template <> inline const smatrix &QGate::matrix<smatrix>() const {
  return sparse_matrix;
}

/// Gets a gate by its name, or std::nullopt if such a gate does not exist
std::optional<QGate*> getGate(std::string gate);

/// @{
/// Standard gate objects
extern QGate ID;
extern QGate X;
extern QGate Y;
extern QGate Z;
extern QGate H;
extern QGate S;
extern QGate T;
extern QGate Tinv;
extern QGate V;
extern QGate Vinv;
extern QGate CNOT;
extern QGate CY;
extern QGate CZ;
extern QGate SWAP;
extern QGate CSWAP;
extern QGate CCNOT;
/// @}

/// Universal gates
extern std::vector<QGate *> uGates;
/// Frees the memory used by universal gates
extern void deleteGates();

/// @{
/// Universal gate constructors
extern QGate* U1(double lambda);
extern QGate* U2(double phi, double lambda);
extern QGate* U3(double theta, double phi, double lambda);
extern QGate* CCU1(double lambda);
extern QGate* CCU2(double phi, double lambda);
extern QGate* CCU3(double theta, double phi, double lambda);
/// @}

} // namespace qc
