#include <qreg.hpp>

namespace qc {

void QRegister::applyOperator(QGate gate, int qubit) {
	std::vector<std::reference_wrapper<const dmatrix>> matrices;
	for(int i = 0; i < nqubits; i++) {
		if(i==qubit) {
			matrices.push_back(std::cref(gate.matrix));
		} else if(i<qubit||i>=qubit+gate.qubits) {
			matrices.push_back(std::cref(ID.matrix));
		}
	}
	gsl::span<const std::reference_wrapper<const dmatrix>> s = gsl::make_span(matrices);
	dmatrix quantumGate = kronecker_dense(s);
	state=quantumGate*state;
}

std::vector<int> QRegister::measureState() {
	std::vector<double> norm(state.rows);
	for(complex c : state.data)norm.push_back(std::norm(c));
	std::random_device rd;
    std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0,1);
	double random = dis(gen);
	int state = 0;
	for(int i = 0; i < norm.size(); i++) {
		random-=norm[i];
		if(random<=0||i==norm.size()-1) {
			state=i;
			break;
		}
	}
	std::vector<int> states;
	for(int i = 0; i < nqubits; i++) {
		states.push_back((state>>(nqubits-1-i))&1);
	}
	return states;
}

std::vector<double> QRegister::measureMultiple(int shots) {
	std::vector<double> result(nqubits);
	for(int i = 0; i < shots; i++) {
		std::vector state = measureState();
		for(int j = 0; j < state.size(); j++) {
			result[j]+=state[j]/(double)shots;
		}
	}
	return result;
}
}
