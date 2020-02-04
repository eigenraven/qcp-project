#include <vector>
#include <qreg.hpp>

namespace qc {

class QCircuit {
	QRegister qreg;
	std::vector<std::pair<int,QGate>> gates;

	public:
	QCircuit(int qbits)
	: qreg(qbits) {}

	void id(int qbit) {
		gates.push_back(std::make_pair(qbit,ID));
	}

	void h(int qbit) {
		gates.push_back(std::make_pair(qbit,H));
	}

	void x(int qbit) {
		gates.push_back(std::make_pair(qbit,X));
	}

	void y(int qbit) {
		gates.push_back(std::make_pair(qbit,Y));
	}
	
	void z(int qbit) {
		gates.push_back(std::make_pair(qbit,Z));
	}

	// So far only gates that operate on single or multiple consecutive QBits have been implemented
	// So this emulates a quantum computer with the QBits connected linearly
	// i.e. Each Qbit is only directly connected to its nearest neighbours
	void cnot(int qbit1, int qbit2) {
		if(qbit1==qbit2)throw std::invalid_argument("This gate must operate on different qbits");
		int lowest = std::min(qbit1,qbit2);
		int highest = std::max(qbit1,qbit2);
		for(int i = highest-1; i > lowest; i--) {
			swapNext(i);
		}
		if(qbit2>qbit1) {
			gates.push_back(std::make_pair(lowest,CNOT));
		} else {
			h(lowest);
			h(lowest+1);
			gates.push_back(std::make_pair(lowest,CNOT));
			h(lowest);
			h(lowest+1);
		}
		for(int i = lowest+1; i < highest; i++) {
			swapNext(i);
		}
	}

	void swapNext(int qbit) {
		gates.push_back(std::make_pair(qbit,SWAP));
	}

	std::vector<double> simulate(int shots) {
		for(std::pair<int,QGate> gate : gates)qreg.applyOperator(gate.second,gate.first);
		return qreg.measureMultiple(shots);
	}
};
}
