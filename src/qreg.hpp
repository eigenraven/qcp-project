#pragma once

#include <vector>
#include <functional>
#include <random>
#include <qgate.hpp>
#include "numeric/numeric.hpp"

namespace qc {

class QRegister {
	public:
	int nqbits;
	dmatrix state;
	QRegister(int nqbits)
	: nqbits(nqbits), state(1<<nqbits,1) {
		state.data[0]=1;
	}

	public:
	void applyOperator(QGate gate, int qbit);

	std::vector<int> measureState();

	std::vector<double> measureMultiple(int shots);
};
}
