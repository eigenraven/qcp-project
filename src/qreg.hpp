#pragma once

#include <vector>
#include <gsl/gsl>
#include <functional>
#include "numeric/numeric.hpp"
#include "qgate.hpp"

namespace qc {

class QRegister {
	public:
	int nqbits;
	dmatrix state;
	inline QRegister(int nqbits)
	: nqbits(nqbits), state(1<<nqbits,1) {
		//verify bounds
	}

	public:
	void applyOperator(QGate gate, int qbit) {
		std::vector<std::reference_wrapper<const dmatrix>> matrices;
		for(int i = 0; i < nqbits; i++) {
			if(i==qbit) {
				matrices.push_back(std::cref(gate.matrix));
			} else {
				matrices.push_back(std::cref(identity.matrix));
			}
		}
		gsl::span<const std::reference_wrapper<const dmatrix>> s = gsl::make_span(matrices);
		dmatrix quantumGate = kronecker_dense(s);
		state=quantumGate*state;
	}
};
}
