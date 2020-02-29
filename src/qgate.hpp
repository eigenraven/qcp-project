#pragma once

#include "numeric/numeric.hpp"
#include <gsl/gsl>
#include <math.h>

namespace qc {

class QGate {
	public:
	int qbits;
	const dmatrix matrix;
	inline QGate(int qbits, std::initializer_list<complex> cdata)
	: qbits(qbits), matrix(1<<qbits,1<<qbits,cdata) {}
};

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
