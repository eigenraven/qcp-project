#pragma once

#include "numeric/numeric.hpp"
#include <gsl/gsl>
#include <math.h>
#include <string>

namespace qc {

class QGate {
	public:
	int qubits;
	const dmatrix matrix;
	inline QGate(int qubits, std::initializer_list<complex> cdata)
	: qubits(qubits), matrix(1<<qubits,1<<qubits,cdata) {}
};


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
}
