#pragma once

#include "numeric/numeric.hpp"
#include <gsl/gsl>

namespace qc {

class QGate {
	public:
	int qbits;
	const dmatrix matrix;
	inline QGate(int qbits, std::initializer_list<complex> cdata)
	: qbits(qbits), matrix(1<<qbits,1<<qbits,cdata) {
		//verify bounds
	}
};

QGate identity(1,{1,0,0,1});
QGate hadamard(1,{1,1,1,-1});
}
