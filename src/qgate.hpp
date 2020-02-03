#pragma once

#include "numeric/numeric.hpp"
#include <gsl/gsl>

namespace qc {

class QGate {
	public:
	const dmatrix matrix;
	inline QGate(int qbits, std::initializer_list<complex> cdata)
	: matrix(1<<qbits,1<<qbits,cdata) {
		//verify bounds
	}
};

QGate hadamard(1,{1,1,1,-1});
}
