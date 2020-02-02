#pragma once

#include "numeric/numeric.hpp"
#include <qgate>

namespace qc {

class qregister {
	public:
	int nqbits;
	dmatrix state;
	inline qregister(int nqbits)
	: nqbits(nqbits), state(1<<nqbits,1) {
		//verify bounds
	}

	public:
	void applyOperator(qgate gate, int qbit) {
		dmatrix quantumGate(1<<nqbits,1<<nqbits);
		for(int i = 0; i < qbit; i++) {
			
		}
	}
};
}
