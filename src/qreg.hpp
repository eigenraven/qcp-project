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
				matrices.push_back(std::ref(gate.matrix));
			} else {
				const dmatrix a = dmatrix::identity(1<<nqbits,1<<nqbits);
				matrices.push_back(std::ref(a));
			}
		}		
		gsl::span<const std::reference_wrapper<const dmatrix>> s = gsl::make_span(matrices);
		dmatrix quantumGate = kronecker_dense(s);
		state=quantumGate*state;
	}
};
}
