#pragma once

#include <vector>
#include <functional>
#include <random>
#include <qgate.hpp>
#include "numeric/numeric.hpp"

namespace qc {

class QRegister {
	public:
	int nqubits;
	dmatrix state;
	QRegister(int nqubits)
	: nqubits(nqubits), state(1<<nqubits,1) {
		state.data[0]=1;
	}

	public:
	void applyOperator(QGate gate, int qubit);

	std::vector<int> measureState();

	std::vector<double> measureMultiple(int shots);
};
}
